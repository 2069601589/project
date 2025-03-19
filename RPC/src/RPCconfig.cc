#include "./include/RPCconfig.h"
#include<memory>

void RPCconfig::loadConfig(const char *configFile)
{
    // 用智能指针管理文件指针
    std::unique_ptr<FILE, decltype(&fclose)> pf(
        fopen(config_file, "r"),  // 打开配置文件
        &fclose  // 文件关闭函数
    );
    // 打开失败
    if(!pf) return exit(EXIT_FAILURE);

    char buf[1024];
    // 读取配置文件
    while (fgets(buf, 1024, pf.get()) != nullptr)
    {
        std::string read_buf=buf;
        Trim(read_buf);
        //注释行跳过
        if (read_buf[0] == '#' || read_buf.empty())
            continue;
        //没有=号的行跳过
        size_t pos = read_buf.find("=");
        if (pos == std::string::npos)
            continue;
        std::string key = read_buf.substr(0, pos);
        std::string value = read_buf.substr(pos + 1);
        if(!key.empty())  configMap[key] = value;
    }
}

std::string RPCconfig::load(const std::string &key)
{
    auto it = configMap.find(key);
    if (it != configMap.end())
    {
        return it->second;
    }
    return "";
}

void RPCconfig::Trim(std::string &str) {
    str.erase(0, str.find_first_not_of(" \t\n\r"));
    str.erase(str.find_last_not_of(" \t\n\r") + 1);
}