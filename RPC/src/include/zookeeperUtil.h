<<<<<<< HEAD
int x
=======
#ifndef ZOOKEEPERUTIL_H
#define ZOOKEEPERUTIL_H
#include <zookeeper/zookeeper.h>
#include <string>
#include <semaphore.h>

class ZkClient
{
public:
    ZkClient();
    ~ZkClient();
    void Start();
    void Create(char *path, char *data, int datalen, int state = 0);
    std::string GetData(char *path);

private:
    zhandle_t *zkhandle;
};
#endif
>>>>>>> develop
