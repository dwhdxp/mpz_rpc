#include <iostream>
#include "mpzrpcapplication.h"
#include "mpzrpccontroller.h"
#include "friend.pb.h"

int main(int argc, char** argv) {
    // 初始化RPC框架
    MpzrpcApplication::Init(argc, argv);

    fixbug::FriendServiceRpc_Stub stub(new MpzrpcChannel());
    fixbug::GetFriendListsRequest request;
    request.set_userid(100);
    fixbug::GetFriendListsResponse response;

    // rpc调用状态信息，可以判断rpc调用是否成功以及输出失败信息
    MpzrpcController controller;
    
    stub.GetFriendLists(&controller, &request, &response, nullptr);

    if (controller.Failed()) {
        std::cout << controller.ErrorText() << std::endl;
    }
    else {
        if (response.result().errcode() == 0) {
            std::cout << "rpc GetFriendLists Response success" << std::endl;
            int n = response.friends_size();
            for (int i = 0; i < n; ++i) {
                std::cout << "index:" << i + 1 << " name: " << response.friends(i) << std::endl;
            }
        } 
        else {
            std::cout << "rpc GetFriendLists Response error: " << response.result().errmsg() << std::endl;
        }
    }

    return 0;
}