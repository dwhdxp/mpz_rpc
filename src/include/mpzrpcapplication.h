#pragma once
#include "mpzrpcconfig.h"


// RPC框架基础类：单例
class MpzrpcApplication {
public:
    static void Init(int argc, char** argv);
    static MpzrpcApplication& GetInstance();

private:
    static MpzrpcConfig m_config;
    MpzrpcApplication() {}
    MpzrpcApplication(const MpzrpcApplication&) = delete;
    MpzrpcApplication(MpzrpcApplication&&) = delete;
};