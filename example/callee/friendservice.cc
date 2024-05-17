#include <iostream>
#include <string>
#include <vector>
#include "friend.pb.h"
#include "mpzrpcapplication.h"
#include "rpcprovider.h"

class FriendService : public fixbug::FriendServiceRpc {
public:
    std::vector<std::string> GetFriendLists(uint32_t userid) {
        std::cout << "do GetFriendLists Func! userid: " << userid << std::endl;
        std::vector<std::string> vec;
        vec.push_back("dwh");
        vec.push_back("dxp");
        vec.push_back("lsx"); 
        return vec;
    }

    void GetFriendLists(::google::protobuf::RpcController* controller,
                       const ::fixbug::GetFriendListsRequest* request,
                       ::fixbug::GetFriendListsResponse* response,
                       ::google::protobuf::Closure* done) {
        uint32_t userid = request->userid();
        std::vector<std::string> friendLists = GetFriendLists(userid);

        response->mutable_result()->set_errcode(0);
        response->mutable_result()->set_errmsg("");
        for (auto &str : friendLists) {
            std::string *s = response->add_friends();
            *s = str;
        }
        done->Run();
    }
};

int main(int argc, char** argv) {
    MpzrpcApplication::Init(argc, argv);

    RpcProvider provider;
    provider.NotifyService(new FriendService());
    provider.Run();

    return 0;
}