#include <iostream>
#include <string>
#include "user.pb.h"
#include "mpzrpcapplication.h"
#include "rpcprovider.h"

// UserService是一个本地服务，提供本地方法Login()
class UserService : public fixbug::UserServiceRpc {
    bool Login(std::string name, std::string pwd) {
        std::cout << "doing local Login() Func" << std::endl;
        std::cout << "name = " << name << " pwd = " << pwd << std::endl;
        return true;
    }

    bool Register(uint32_t id, std::string name, std::string pwd) {
        std::cout << "doing local Register() Func" << std::endl;
        std::cout << "id = " << id << " name = " << name << " pwd = " << pwd << std::endl;
        return true;
    }

    // 重写基类中的Login()虚函数
    void Login(::google::protobuf::RpcController *controller,
               const ::fixbug::LoginRequest *request,
               ::fixbug::LoginResponse *response,
               ::google::protobuf::Closure *done) {
        // 1.获取rpc框架上报的request参数
        std::string name = request->name();
        std::string pwd = request->pwd();

        // 2.使用request参数做本地业务
        bool res = Login(name, pwd);

        // 3.写入response数据
        fixbug::ResultCode* rc = response->mutable_result();
        rc->set_errcode(0);
        rc->set_errmsg("");
        response->set_success(res);

        // 4.执行回调操作，将response数据序列化并通过网络发送
        done->Run();
    }

    // 重写Register()虚函数
    void Register(::google::protobuf::RpcController* controller,
                  const ::fixbug::RegisterRequest* request,
                  ::fixbug::RegisterResponse* response,
                  ::google::protobuf::Closure* done) {
        uint32_t id = request->id();
        std::string name = request->name();
        std::string pwd = request->pwd();

        bool res = Register(id, name, pwd);
        response->mutable_result()->set_errcode(0);
        response->mutable_result()->set_errmsg("");
        response->set_success(res);
        done->Run();
    }  
};

int main(int argc, char** argv) {
    // 初始化rpc框架
    MpzrpcApplication::Init(argc, argv);

    RpcProvider provider;
    // 部署rpc服务到rpc节点
    provider.NotifyService(new UserService());
    // 启动rpc服务节点，开始提供rpc远程调用服务
    provider.Run();

    return 0;
}