#pragma once
#include <iostream>
#include <semaphore.h>
#include <string>
#include "zookeeper/zookeeper.h"
#include "mpzrpcapplication.h"
#include "logger.h"

class ZkClient {
public:
    ZkClient();

    ~ZkClient();

    // zkclient启动连接zkserver
    void Start();

    // 在zkserver上根据指定的path创建znode
    void Create(const char *path, const char *data, int datalen, int state = 0);

    // 根据指定的znode节点路径，查找znode值
    std::string GetData(const char *path);
private:
    // zookeeper service连接的句柄
    zhandle_t *m_zhandle;
};