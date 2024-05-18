#include "mpzrpccontroller.h"

MpzrpcController::MpzrpcController() : _failed(false), _errText("") {}

void MpzrpcController::Reset() {
    _failed = false;
    _errText = "";
}

bool MpzrpcController::Failed() const {
    return _failed;
}

std::string MpzrpcController::ErrorText() const {
    return _errText;
}

void MpzrpcController::SetFailed(const std::string& reason) {
    _failed = true;
    _errText = reason;
}

void MpzrpcController::StartCancel() {}
bool MpzrpcController::IsCanceled() const {}
void MpzrpcController::NotifyOnCancel(google::protobuf::Closure* callback) {}