#ifndef _RPCapplication_H
#define _RPCapplication_H
#include "RPCconfig.h"
#include <mutex>
class RPCapplication
{
public:
    static void Init(int argc,char **argv);
    static RPCapplication &GetInstance();
    static void deleteInstance();
    static RPCconfig &getConfig();

private:
    static RPCconfig m_config;
    static RPCapplication *m_application;
    static std::mutex m_mutex;
    RPCapplication();
    ~RPCapplication();
    RPCapplication(const RPCapplication&)=delete;
    RPCapplication(RPCapplication&&)=delete;
};

#endif