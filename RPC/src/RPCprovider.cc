#include "./include/RPCapplication.h"
#include "./include/RPCprovider.h"
#include "./include/RPCheader.pb.h"
#include <iostream>

RPCprovider::~RPCprovider()
{
    std::cout << "退出" << std::endl;
    event_loop.quit();
};
//提供给外部使用，注册服务对象和方法
void RPCprovider::NotifySevice(google::protobuf::Service *service)
{
    //参数为protobuf派生类的基类，可以使用一个基类指针指向不同的派生类，实现动态多态
    //通过service_info保存服务的对象和方法的名称
    ServiceInfo service_info;
    //GetDescriptor()获取服务类的信息
    const google::protobuf::ServiceDescriptor *psd = service->GetDescriptor();
    //服务名
    std::string service_name = psd->name();
    std::cout << "service_name:" << service_name << std::endl;
    //方法数量
    int method_count = psd->method_count();
    for (int i = 0; i < method_count; i++)
    {
        const google::protobuf::MethodDescriptor *pmd = psd->method(i);
        //获取方法名
        std::string method_name = pmd->name();
        std::cout << "method_name:" << method_name << std::endl;
        service_info.method_map.emplace(method_name, pmd);
    }
    service_info.service = service;
    //记录所有的服务对象和方法
    service_map.emplace(service_name, service_info);
};

void RPCprovider::Run()
{
    //获取ip和端口号
    std::string ip = RPCapplication::GetInstance().getConfig().load("rpcserverip");
    int port = stoi(RPCapplication::GetInstance().getConfig().load("rpcserverport"));
    //创建地址对象
    muduo::net::InetAddress address(ip, port);
    //share_ptr创建server对象
    std::shared_ptr<muduo::net::TcpServer> server = std::make_shared<muduo::net::TcpServer>(&event_loop, address, "rpcProvider");
    //设置回调函数
    server->setConnectionCallback(std::bind(&RPCprovider::onConnection, this, std::placeholders::_1));
    server->setMessageCallback(std::bind(&RPCprovider::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    //设置线程数量
    server->setThreadNum(4);

    // 把RPC的服务全部注册到zookeeper上
    ZkClient zkClient;
    //连接zookeeper服务器
    zkClient.Start();
    for (auto &sp : service_map)
    {
        //服务名
        std::string service_path = "/" + sp.first;
        //为每个服务创建一个节点
        //服务是永久节点
        zkClient.Create(service_path.c_str(), nullptr, 0);
        for (auto &mp : sp.second.method_map)
        {
            //方法路径
            std::string method_path = service_path + "/" + mp.first;
            char method_path_data[128] = {0};
            sprintf(method_path_data, "%s:%d", ip.c_str(), port); // 将IP和端口信息存入节点数据
            //方法是临时节点，服务器断开就注销，防止客户端获取到不可用节点
            zkClient.Create(method_path.c_str(), method_path_data, strlen(method_path_data), ZOO_EPHEMERAL);
        }
    }
    std::cout << "RpcProvider start service at ip:" << ip << " port:" << port << std::endl;
    // 启动网络服务
    server->start();
    event_loop.loop();
};

void RPCprovider::onConnection(const muduo::net::TcpConnectionPtr &conn)
{
    if (!conn->connected())
    {
        conn->shutdown();
    }
};

void RPCprovider::onMessage(const muduo::net::TcpConnectionPtr &conn, muduo::net::Buffer *buffer, muduo::Timestamp recive_time)
{

    std::cout << "onMessage" << std::endl;
    std::string recv_buf = buffer->retrieveAllAsString();

    // 创建字节流
    google::protobuf::io::ArrayInputStream raw_input(recv_buf.data(), recv_buf.size());
    // 创建编码流
    google::protobuf::io::CodedInputStream code_input(&raw_input);

    uint32_t header_size{};
    code_input.ReadVarint32(&header_size);

    std::string rpc_header_str;
    RPC::RPCheader rpcHeader;
    std::string service_name;
    std::string method_name;
    uint32_t args_size{};

    google::protobuf::io::CodedInputStream::Limit msg_limit = code_input.PushLimit(header_size);
    code_input.ReadString(&rpc_header_str, header_size);
    code_input.PopLimit(msg_limit);

    if (rpcHeader.ParseFromString(rpc_header_str))
    {
        service_name = rpcHeader.service_name();
        method_name = rpcHeader.method_name();
        args_size = rpcHeader.args_size();
    }
    else
    {
        std::cout << "header parse error" << std::endl;
    }

    std::string args_str;
    bool read_args_success = code_input.ReadString(&args_str, args_size);
    if (!read_args_success)
    {
        std::cout << "read args error" << std::endl;
        return;
    }

    auto it = service_map.find(service_name);
    if (it == service_map.end())
    {
        std::cout << "service_name 不存在" << std::endl;
        return;
    }
    auto mit = it->second.method_map.find(method_name);
    if (mit == it->second.method_map.end())
    {
        std::cout << "method_name 不存在" << std::endl;
        return;
    }

    google::protobuf::Service *service = it->second.service;
    const google::protobuf::MethodDescriptor *method = mit->second;

    google::protobuf::Message *request = service->GetRequestPrototype(method).New();
    if (!request->ParseFromString(args_str))
    {
        return;
    }
    google::protobuf::Message *response = service->GetResponsePrototype(method).New();

    // 绑定回调函数，用于在方法调用完成后发送响应
    google::protobuf::Closure *done = google::protobuf::NewCallback
    <RPCprovider,const muduo::net::TcpConnectionPtr &,google::protobuf::Message *>
    (this,&RPCprovider::sendRPCresponse,conn, response);

    service->CallMethod(method, nullptr, request, response, done);
};

void RPCprovider::sendRPCresponse(const muduo::net::TcpConnectionPtr &conn, google::protobuf::Message *response)
{
    std::string response_str;
    if (response->SerializeToString(&response_str))
    {
        conn->send(response_str);
    }
    else
    {
        std::cout << "反序列化失败" << std::endl;
    }
};