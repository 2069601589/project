#ifndef _RPCprovider_h
#define _RPCprovider_h

#include "google/protobuf/service.h"
#include "zookeeperUtil.h"
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/TcpConnection.h>
#include <google/protobuf/descriptor.h>
#include <functional>
#include <string>
#include <unordered_map>

class RPCprovider
{
public:
    RPCprovider()=default;
    ~RPCprovider();
    void NotifySevice(google::protobuf::Service *service);
    void Run();
private:
    muduo::net::EventLoop event_loop;
    struct ServiceInfo
    {
        google::protobuf::Service *service;
        std::unordered_map<std::string,const google::protobuf::MethodDescriptor*> method_map;
    };
    std::unordered_map<std::string,ServiceInfo> service_map;

    void onConnection(const muduo::net::TcpConnectionPtr &conn);
    void onMessage(const muduo::net::TcpConnectionPtr  &conn,muduo::net::Buffer* buffer,muduo::Timestamp recive_time);
    void sendRPCresponse(const muduo::net::TcpConnectionPtr &conn,google::protobuf::Message* response);
};

#endif