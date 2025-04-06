#ifndef _RPCcontroller_h
#define _RPCcontroller_h

#include <google/protobuf/service.h>
#include <string>

class RPCcontroller : public ::google::protobuf::RpcController
{
private:
    bool m_failed;
    std::string m_errText;

public:
    RPCcontroller();
    ~RPCcontroller() = default;
    void Reset();
    bool Failed() const;
    void SetFailed(const std::string &reason);
    std::string ErrorText() const;

    //未实现
    void StartCancel();
    bool IsCanceled() const;
    void NotifyOnCancel(google::protobuf::Closure *callback);
};

#endif