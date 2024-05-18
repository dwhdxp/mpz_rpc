#pragma once

#include <string>
#include "google/protobuf/service.h"

// 负责返回rpc调用的状态信息
class MpzrpcController : public google::protobuf::RpcController {
public:
    MpzrpcController();
    // 重置状态
    void Reset();

    // 判断rpc调用是否成功
    bool Failed() const;

    // 打印rpc调用状态信息
    std::string ErrorText() const;

    // 设置rpc调用失败原因
    void SetFailed(const std::string& reason);

    void StartCancel();
    bool IsCanceled() const;
    void NotifyOnCancel(google::protobuf::Closure* callback);

private:
    bool _failed; // rpc调用标志
    std::string _errText; // rpc调用状态信息
};
