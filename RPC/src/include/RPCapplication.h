#ifndef _RPCapplication_H
#define _RPCapplication_H
#include "RPCconfig.h"
#include "RPCchannel.h"
#include  "RPCcontroller.h"
#include <mutex>
class RPCapplication
{
public:
    static void Init(int argc,char **argv); //初始化
    static RPCapplication &GetInstance();  //获取实例
    static void deleteInstance();  //注销实例
    static RPCconfig &getConfig();  //获取配置

private:
    static RPCconfig m_config;
    static RPCapplication *m_application;
    static std::mutex m_mutex;
private:
    //单例模式，将构造和析构都放入private
    RPCapplication(){}
    ~RPCapplication(){}
    RPCapplication(const RPCapplication&)=delete;
    RPCapplication(RPCapplication&&)=delete;
};

#endif