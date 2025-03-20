#include "./include/RPCchannel.h"
#include "./include/RPCheader.pb.h"
#include "./include/zookeeperUtil.h"
#include "./include/RPCapplication.h"
#include "./include/RPCcontroller.h"
#include "memory"
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

std::mutex g_data_mutex;
RPCchannel::RPCchannel(bool connectNow):m_clientfd(-1),m_idex(0) {
    if(!connectNow) return;
    auto rt=NewConnect(m_ip.c_str(),m_port);
    int count=3;
    //失败重连，最多三次
    while(!rt&&count--){
        rt=NewConnect(m_ip.c_str(),m_port);
    }
};

void RPCchannel::CallMethod(const google::protobuf::MethodDescriptor *method,
                            google::protobuf::RpcController *controller,
                            const google::protobuf::Message *request,
                            google::protobuf::Message *response,
                            google::protobuf::Closure *done)
{
    //socket还未进行初始化，进行连接
   if(-1==m_clientfd){
        const google::protobuf::ServiceDescriptor *sd=method->service();
        service_name=sd->name();
        method_name = method->name();
         
        //在zookeeper中获取ip和port
        ZkClient zkcli;
        zkcli.Start();
        std::string host_data=QueryServerHost(&zkcli,service_name,method_name,m_idex);
        m_ip=host_data.substr(0,m_idex);
        m_port=atoi(host_data.substr(m_idex+1,host_data.size()-m_idex).c_str());

        auto rt=NewConnect(m_ip.c_str(),m_port);
        if(!rt) return;
   }
   uint32_t args_size{};
   std::string args_str;
   if(request->SerializeToString(&args_str)){
    args_size=args_str.size();
   }else return;

   RPC::RPCheader rpc_head;
   rpc_head.set_service_name(service_name);
   rpc_head.set_method_name(method_name);
   rpc_head.set_args_size(args_size);

   uint32_t head_size;
   std::string rpc_head_str;
   if(rpc_head.SerializeToString(&rpc_head_str)){
    head_size=rpc_head_str.size();
   }else return;

   //拼接完整的RPC报文
   std::string send_rpc_str;
   {
    google::protobuf::io::StringOutputStream string_output(&send_rpc_str);
    google::protobuf::io::CodedOutputStream code_output(&string_output);
    code_output.WriteVarint32(static_cast<uint32_t> (head_size));
    code_output.WriteString(rpc_head_str);
   }
   send_rpc_str+=args_str;

   //发送RPC请求到服务器
   if(-1==send(m_clientfd,send_rpc_str.c_str(),send_rpc_str.size(),0)){
     close(m_clientfd);
     return;
   }

   //接收服务器响应
   char recv_buf[1024];
   int recv_size=0;
   if(-1==(recv_size=recv(m_clientfd,recv_buf,1024,0))){
     return;
   }

   //将响应数据反序列化为响应对象
   if(!response->ParseFromArray(recv_buf,recv_size)){
      close(m_clientfd);
      return;
   }
   close(m_clientfd);

};

bool RPCchannel::NewConnect(const char *ip, uint16_t port){
    int clientfd=socket(AF_INET,SOCK_STREAM,0);
    if(clientfd==-1) return false;
    struct sockaddr_in server_addr;
    server_addr.sin_family=AF_INET;
    server_addr.sin_port=htons(port);
    server_addr.sin_addr.s_addr=inet_addr(ip);
    if(-1==connect(clientfd,(struct sockaddr*)&server_addr,sizeof(server_addr))){
        close(clientfd);
        return false;
    }
    m_clientfd=clientfd;
    return true;
};

std::string RPCchannel::QueryServerHost(ZkClient *zkClient, std::string service_name, std::string method_name, int &idex){
    std::string method_path="/"+service_name+"/"+method_name;
    std::cout<<"method_path: "<<method_path<<std::endl;
    std::unique_lock lock(g_data_mutex);
    std::string host_data=zkClient->GetData(method_path.c_str());
    lock.unlock();
    if(host_data=="") return "";
    idex=host_data.find(":"); 
    if(idex==-1) return "";
    return host_data;
};