#include <iostream>
#include <string>
#include <unistd.h>
#include "mpzrpcapplication.h"

// static member
MpzrpcConfig MpzrpcApplication::m_config;

void ShowArgHelp() {
    std::cout << "format：command -i <configfile>" << std::endl;
}

void MpzrpcApplication::Init(int argc, char** argv) {
    if (argc < 2) {
        ShowArgHelp();
        exit(EXIT_FAILURE);
    }

    // 参数解析
    int c = 0;
    std::string config_file;
    const char* optstring = "i:"; // 包含正确参数选项的字符串
    while ((c = getopt(argc, argv, optstring)) != -1) {
        switch (c) {
        case 'i':
            config_file = optarg; // 获得配置文件string
            break;
        case '?':
            ShowArgHelp();
            exit(EXIT_FAILURE);
        case ':':
            ShowArgHelp();
            exit(EXIT_FAILURE);
        default:
            break;
        }
    }

    // 开始加载配置文件
    m_config.LoadConfigFile(config_file.c_str());
    // std::cout << "rpcserverip:" << m_config.Load("rpcserverip") << std::endl;
    // std::cout << "rpcserverport:" << m_config.Load("rpcserverport") << std::endl;
    // std::cout << "zookeeperip:" << m_config.Load("zookeeperip") << std::endl;
    // std::cout << "zookeeperport:" << m_config.Load("zookeeperport") << std::endl;   
}

MpzrpcApplication& MpzrpcApplication::GetInstance() {
    static MpzrpcApplication app;
    return app;
}

MpzrpcConfig& MpzrpcApplication::GetConfig() {
    return m_config;
}