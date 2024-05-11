#pragma once
#include "google/protobuf/service.h"

// 框架提供的专门服务rpc服务的网络对象类
class RpcProvider {
public:
    // RPC框架提供的，可以用于发布rpc方法的接口
    void NotifyService(google::protobuf::Service *service);
    
    // 启动rpc服务节点，开始提供rpc远程调用服务
    void Run();
private:

};