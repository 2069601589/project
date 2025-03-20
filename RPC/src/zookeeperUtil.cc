#include "./include/zookeeperUtil.h"
#include "./include/RPCapplication.h"
#include<mutex>
#include<condition_variable>

std::mutex cv_mutex;
std::condition_variable cv;
bool isConnect=false;

//全局观察器
void global_watcher(zhandle_t* zh,int type,int status,const char*path,void *watcherCtx){
    if(type==ZOO_SESSION_EVENT){
        if(status==ZOO_CONNECTED_STATE){
            std::lock_guard<std::mutex> lock(cv_mutex);
            isConnect=true;
        }
    }
    cv.notify_all(); //通知所有等待线程
}

ZkClient::ZkClient():zkhandle(nullptr){};
ZkClient:: ~ZkClient(){
    if(zkhandle){
        zookeeper_close(zkhandle);
    }
};
void ZkClient::Start(){
    std::string host=RPCapplication::GetInstance().getConfig().load("zookeeperip");
    std::string port=RPCapplication::GetInstance().getConfig().load("zookeeperport");
    std::string connstr=host+":"+port;

    zkhandle=zookeeper_init(connstr.c_str(), global_watcher,6000,nullptr,nullptr,0);
    if(zkhandle==nullptr){
        std::cout<<"zookeeper初始化失败"<<std::endl;
        exit(EXIT_FAILURE);
    }
    std::unique_lock<std::mutex> lock(cv_mutex);
    cv.wait(lock,[]{return isConnect;});  //阻塞线程
    std::cout<<"zookeeper初始化成功"<<std::endl;

};


void ZkClient::Create(char *path, char *data, int datalen, int state = 0){
    char path_buffer[128];
    int bufferlen=sizeof(path_buffer);

    int flag=zoo_exists(zkhandle,path,0,nullptr);
    if(flag==ZNONODE){
       flag=zoo_create(zkhandle,path, data, datalen, &ZOO_OPEN_ACL_UNSAFE, state, path_buffer, bufferlen);
       if(flag=ZOK){
         std::cout<<"创建节点成功"<<std::endl;
       }else{
        std::cout<<"创建节点失败"<<std::endl;
         exit(EXIT_FAILURE);
       }
    } 
};
std::string ZkClient:: GetData(char *path){
     char buf[64];
     int buflen=sizeof(buf);

     int flag=zoo_get(zkhandle,path,0,buf,&buflen,nullptr);
     if(flag==ZOK){
        std::cout<<"获取成功"<<std::endl;
        return buf;
     }else{
        std::cout<<"获取失败"<<std::endl;
     }
     return "";
};
