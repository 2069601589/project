#ifndef _RPCcontroller_h
#define _RPCcontroller_h
#include<google/protobuf/service.h>
#include<string>

class RPCcontroller:public::google::protobuf::RpcController
{
private:
    bool m_failed;
    std::string m_errText;
public:
    RPCcontroller();
    ~RPCcontroller()=default;
    void Reset();
    bool Failed();
    void SetFailed(const std::string&reason);
    std::string ErrText();
};


#endif