#include "mpzrpcconfig.h"

void MpzrpcConfig::LoadConfigFile(const char* config_file) {
    FILE *fp = fopen(config_file, "r");
    if (fp == nullptr) {
        std::cout << "read config_file fail" << std::endl;
        exit(EXIT_FAILURE);
    }

    while (!feof(fp)) {
        char buf[512] = {0};
        // 获取配置文件中的一行
        fgets(buf, 512, fp);

        // 处理行字符串：去除多余空格；去除注释行；
        std::string read_buf(buf);
        // 去除整个配置项的前后空格
        TrimStr(read_buf);
        
        // 字符串为注释行或为空时，继续读取下一行
        if (read_buf[0] == '#' || read_buf.empty()) {
            continue;
        }

        // 解析配置项内容
        int idx = read_buf.find('=');
        if (idx == -1) {
            continue;
        }
        std::string key;
        std::string value;
        
        // key = reserverip
        key = read_buf.substr(0, idx);
        TrimStr(key);
        
        // 去掉末尾换行符
        int endidx = read_buf.find('\n');
        value = read_buf.substr(idx + 1, endidx - idx - 1);
        TrimStr(value);
        m_configMap.insert({key, value});
    }
}

std::string MpzrpcConfig::Load(const std::string &key) {
    auto it = m_configMap.find(key);
    if (it == m_configMap.end()) {
        return nullptr;
    }
    return it->second;
}

void MpzrpcConfig::TrimStr(std::string &str_buf) {
    // 去除前面多余的空格
    int idx = str_buf.find_first_not_of(' '); // 从前向后找到第一个不是空格的位置
    if (idx != -1 && idx != 0) {
        str_buf = str_buf.substr(idx, str_buf.size() - idx);
    }
    
    // 去除后面多余的空格
    idx = str_buf.find_last_not_of(' ');
    if (idx != -1 && idx != str_buf.size() - 1) {
        str_buf = str_buf.substr(0, idx + 1);
    }
}