#include"./include/RPCapplication.h"
#include<unistd.h>
#include<cstdlib>
RPCconfig RPCapplication::m_config;
RPCapplication* RPCapplication::m_application=nullptr;
std::mutex RPCapplication::m_mutex;

void RPCapplication::Init(int argc,char**argv){
    if(argc<2){
        std::cout<<"格式: command -i <path>"<<std::endl;
        exit(EXIT_FAILURE);
    }
    int o;
    std::string configFile;
    while(-1!=(o=getopt(argc,argv,"i:"))){
        switch (o)
        {
        case 'i':
            configFile=optarg;
            break;
        case '?':
            std::cout<<"格式: command -i <path>"<<std::endl;
            break;
        case ':':
            std::cout<<"格式: command -i <path>"<<std::endl;
        default:
            break;
        }
    }
    m_config.loadConfig(configFile.c_str());

}

RPCapplication &RPCapplication::GetInstance(){
    std::lock_guard<std::mutex> lock(m_mutex);
    if(!m_application){
        m_application=new RPCapplication();
        atexit(deleteInstance);
    }
    return *m_application;
};
void RPCapplication:: deleteInstance(){
    if(m_application){
        delete m_application;
    }
};
RPCconfig &RPCapplication::getConfig(){
   return m_config;
};