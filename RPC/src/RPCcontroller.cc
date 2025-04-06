#include"./include/RPCcontroller.h"

RPCcontroller::RPCcontroller(){
    m_errText="";
    m_failed=false; //初始为未失败
};

void RPCcontroller:: Reset(){
    m_errText="";
    m_failed=false;
};

bool RPCcontroller::Failed() const {return m_failed;};

void RPCcontroller::SetFailed(const std::string&reason){
    m_failed=true;
    m_errText=reason;
};
std::string RPCcontroller::ErrorText() const { return m_errText;};

//todo 取消调用

//未实现
void RPCcontroller::StartCancel(){

}
bool RPCcontroller::IsCanceled() const{
    return false;
}
void RPCcontroller::NotifyOnCancel(google::protobuf::Closure *callback){

}