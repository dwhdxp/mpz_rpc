#include <iostream>
#include <string>
#include "user.pb.h"

// UserService是一个本地服务，提供本地方法Login()
class UserService : public fixbug::UserServiceRpc {
    bool Login(std::string name, std::string pwd) {
        std::cout << "doing local Login() Func" << std::endl;
        std::cout << "name = " << name << " pdw = " << pwd << std::endl;
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
};

int main() {
    
    return 0;
}