#pragma once

#include <iostream>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "google/protobuf/service.h"
#include "google/protobuf/descriptor.h"
#include "google/protobuf/message.h"

#include "rpcheader.pb.h"
#include "mpzrpcapplication.h"

class MpzrpcChannel: public google::protobuf::RpcChannel {
public:
    // Stub将所有方法的调用都指向CallMethod
    // 1. rpc请求序列化，并按消息格式组装rpc request; 2. Tcp网络编程发送rpc请求消息; 3. 将response反序列化返回给client
    void CallMethod(const google::protobuf::MethodDescriptor* method,
                    google::protobuf::RpcController* controller, 
                    const google::protobuf::Message* request,
                    google::protobuf::Message* response, 
                    google::protobuf::Closure* done);
private:
};