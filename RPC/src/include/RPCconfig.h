#ifndef RPCCONFIG_H
#define RPCCONFIG_H

#include <iostream>
#include <string>
#include <map>
class RPCconfig
{
    public:
        void loadConfig(const char *configFile);  //加载配置文件
        std::string load(const std::string &key);  //根据key获取value
    private:
        std::map<std::string, std::string> configMap; //配置文件内容
        void Trim(std::string &read_buf); //去掉字符串前后的空格
};

#endif // RPCCONFIG_H