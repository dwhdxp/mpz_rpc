#pragma once
#include <iostream>
#include <functional>
#include <unordered_map>

#include "muduo/net/TcpServer.h"
#include "muduo/net/EventLoop.h"
#include "muduo/net/InetAddress.h"
#include "muduo/net/Buffer.h"
#include "google/protobuf/service.h"
#include "google/protobuf/descriptor.h"
#include "mpzrpcapplication.h"
#include "rpcheader.pb.h"

// 框架提供的专门服务rpc服务的网络对象类
class RpcProvider {
public:
    // RPC框架提供的，可以用于发布rpc方法的接口
    void NotifyService(google::protobuf::Service *service);
    
    // 启动rpc服务节点，开始提供rpc远程网络调用服务
    void Run();
private:
    muduo::net::EventLoop m_eventLoop;
    
    // 服务相关信息
    struct ServiceInfo {
        google::protobuf::Service *m_service; // 服务对象
        // 服务RPC类中所有的rpc方法，<method_name, method>
        std::unordered_map<std::string,  const google::protobuf::MethodDescriptor*> m_methodMap;
    };
    // 服务类名 ==> 服务相关信息
    std::unordered_map<std::string, ServiceInfo> m_serviceMap;

    // 回调操作
    void SendRpcResponse(const muduo::net::TcpConnectionPtr&, google::protobuf::Message*);
    void OnConnection(const muduo::net::TcpConnectionPtr&);
    void OnMessage(const muduo::net::TcpConnectionPtr&, muduo::net::Buffer*, muduo::Timestamp);
};