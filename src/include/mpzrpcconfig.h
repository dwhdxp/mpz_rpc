#pragma once

#include <iostream>
#include <string>
#include <unordered_map>

// rpc框架读取配置文件类
class MpzrpcConfig {
public:
    // 解析配置文件
    void LoadConfigFile(const char* config_file);
    
    // 查询配置信息
    std::string Load(const std::string &key);

private:
    std::unordered_map<std::string, std::string> m_configMap;
    // 去除多余的前后空格
    void TrimStr(std::string &str_buf);
};