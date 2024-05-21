#pragma once
#include "mpzrpcconfig.h"
#include "mpzrpcchannel.h"
#include "mpzrpccontroller.h"
#include "rpcprovider.h"
#include "logger.h"
#include "zookeeperutil.h"

// RPC框架基础类：单例
class MpzrpcApplication {
public:
    static void Init(int argc, char** argv);
    static MpzrpcApplication& GetInstance();
    static MpzrpcConfig& GetConfig();
private:
    static MpzrpcConfig m_config;
    MpzrpcApplication() {}
    MpzrpcApplication(const MpzrpcApplication&) = delete;
    MpzrpcApplication(MpzrpcApplication&&) = delete;
};