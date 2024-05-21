#include "zookeeperutil.h"

// 全局watcher观察器：zkserver给zkclient通知
void global_watcher(zhandle_t *zh, int type, 
                    int state, const char *path, void *watcherCtx) {
    // 回调的消息类型是和会话相关的消息类型
    if (type == ZOO_SESSION_EVENT) {
        // zkclient和zkserver连接成功
        if (state == ZOO_CONNECTED_STATE) {
            sem_t *sem = (sem_t*)zoo_get_context(zh);
            sem_post(sem);
        }
    }
}

ZkClient::ZkClient() : m_zhandle(nullptr) {}

ZkClient::~ZkClient() {
    if (m_zhandle != nullptr) {
        // 关闭句柄，释放资源
        zookeeper_close(m_zhandle);
    }
}

/*
zhandle_t *zookeeper_init(const char *host, watcher_fn fn,
        int recv_timeout, const clientid_t *clientid, void *context, int flags);
*/
void ZkClient::Start() {
    std::string ip = MpzrpcApplication::GetInstance().GetConfig().Load("zookeeperip");
    std::string port = MpzrpcApplication::GetInstance().GetConfig().Load("zookeeperport");
    std::string host = ip + ":" + port;
    /*
        zookeeper_mt：API客户端程序会提供三个线程
        1. API调用线程；2. 网络I/O线程：poll；3. 调用watcher回调线程:Session establishment is asynchronous
    */
    m_zhandle = zookeeper_init(host.c_str(), global_watcher, 30000, nullptr, nullptr, 0);
    if (m_zhandle == nullptr) {
        LOG_ERROR("zhandle_init error!");
        exit(EXIT_FAILURE);
    }
    
    sem_t sem;
    sem_init(&sem, 0, 0);
    // 给句柄添加上下文
    zoo_set_context(m_zhandle, &sem);

    sem_wait(&sem);
    LOG_INFO("zookeeper_init success!");
}

/*
在zkserver上根据指定的path创建znode，每个znode节点均携带data
state == 0时创建永久性节点，state == ZOO_EPHEMERAL时，创建临时性节点
*/ 
void ZkClient::Create(const char* path, const char* data, int datalen, int state) {
    char pathBuf[128] {0};
    int bufferLen = sizeof(pathBuf);
    // 判断需要创建的znode是否已存在，若已存在则不再重复创建
    int flag = zoo_exists(m_zhandle, path, 0, nullptr);
    if (flag == ZNONODE) {
        // 创建znode
        flag = zoo_create(m_zhandle, path, data, datalen, 
                        &ZOO_OPEN_ACL_UNSAFE, state, pathBuf, bufferLen);
        if (flag == ZOK) {
            LOG_INFO("znode create success in path:%s", path);
        }
        else {
            LOG_ERROR("znode create error in path:%s, flag:%d", path, flag);
            exit(EXIT_FAILURE);
        }
    }
}

// 根据指定的znode节点路径，查找znode值
std::string ZkClient::GetData(const char *path) {
    char buffer[128] {0};
    int bufferlen = sizeof(buffer);
    int flag = zoo_get(m_zhandle, path, 0, buffer, &bufferlen, nullptr);
    if (flag != ZOK) {
        LOG_ERROR("get znode val error in path:%s", path);
    }
    else {
        return buffer;
    }
    return "";
}   