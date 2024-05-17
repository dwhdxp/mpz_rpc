#include <iostream>
#include "mpzrpcapplication.h"
#include "mpzrpcchannel.h"
#include "user.pb.h"

int main(int argc, char** argv) {
    // 初始化RPC框架
    MpzrpcApplication::Init(argc, argv);
    
    // 演示调用UserServiceRpc对象上的Login方法
    fixbug::UserServiceRpc_Stub stub(new MpzrpcChannel());
    
    fixbug::LoginRequest request;
    request.set_name("XZ lsx");
    request.set_pwd("123456");
    fixbug::LoginResponse response;
    stub.Login(nullptr, &request, &response, nullptr);

    if (response.result().errcode() == 0) {
        std::cout << "rpc login success: " << response.success() << std::endl;
    }
    else {
        std::cout << "rpc login response error: " << response.result().errmsg() << std::endl;
    }

    // 调用UserServiceRpc对象上的Register方法
    fixbug::RegisterRequest rq;
    rq.set_id(1000);
    rq.set_name("Dxp");
    rq.set_pwd("123456789");
    fixbug::RegisterResponse rep;
    stub.Register(nullptr, &rq, &rep, nullptr);

    if (rep.result().errcode() == 0) {
        std::cout << "rpc Register success: " << rep.success() << std::endl;
    }
    else {
        std::cout << "rpc Register response error: " << rep.result().errmsg() << std::endl;
    }

    return 0;
}