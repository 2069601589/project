#include"./include/RPCcontroller.h"

RPCcontroller::RPCcontroller(){
    m_errText="";
    m_failed=false; //初始为未失败
};

void RPCcontroller:: Reset(){
    m_errText="";
    m_failed=false;
};

bool RPCcontroller::Failed(){return m_failed;};

void RPCcontroller::SetFailed(const std::string&reason){
    m_failed=true;
    m_errText=reason;
};
std::string RPCcontroller::ErrText(){ return m_errText;};