#ifndef _RPCchannel_h
#define _RPCchannel_h
#include <google/protobuf/service.h>
#include "zookeeperUtil.h"
class RPCchannel:public google::protobuf::RpcChannel
{
public:
    RPCchannel(bool connectNow);
    virtual ~RPCchannel(){}
    void CallMethod(const google::protobuf::MethodDescriptor*method,
                    google::protobuf::RpcController *controller,
                    const google::protobuf::Message *request,
                    google::protobuf::Message *response,
                    google::protobuf::Closure *done) override;
private:
    int m_clientfd; //套接字编号
    std::string service_name;
    std::string m_ip; //ip 
    uint16_t m_port;
    std::string method_name;
    int m_idex; //记录ip和port的分界线
    bool NewConnect(const char *ip, uint16_t port);
    std::string QueryServerHost(ZkClient *zkClient,std::string service_name,std::string method_name,int &idex);
};

#endif