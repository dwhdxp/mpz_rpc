#include "test.pb.h"
#include <iostream>
#include <string>

int main() {
    // 对象数据的使用
    // 设置数据
    fixbug::LoginRequest reqA;
    reqA.set_name("l s x");
    reqA.set_pwd("123");

    // 数据序列化
    std::string str;
    if (reqA.SerializeToString(&str)) {
        std::cout << str << std::endl;
    }

    // 数据反序列化
    fixbug::LoginRequest reqB;
    if (reqB.ParseFromString(str)) {
        std::cout << reqB.name() << std::endl;
        std::cout << reqB.pwd() << std::endl;
    }

    // 数组数据使用
    fixbug::GetFriendListsResponse rsp;
    fixbug::ResultCode* rc = rsp.mutable_result(); // 得到类私有成员的地址
    rc->set_errcode(0);
    
    // 添加数组数据
    fixbug::User *user1 = rsp.add_friendlists();
    user1->set_name("lsx");
    user1->set_age(18);
    user1->set_sex(fixbug::User::WOMAN);
    std::cout << rsp.friendlists_size() << std::endl;

    fixbug::User *user2 = rsp.add_friendlists();
    user2->set_name("dwh");
    user2->set_age(22);
    user2->set_sex(fixbug::User::MAN);
    std::cout << rsp.friendlists_size() << std::endl;

    // 序列化
    std::string str2;
    if (rsp.SerializeToString(&str2)) {
        std::cout << str2 << std::endl;
    }

    // 反序列化
    fixbug::GetFriendListsResponse rsp2;
    if (rsp2.ParseFromString(str2)) {
        int size = rsp2.friendlists_size();
        for (int i = 0; i < size; ++i) {
            fixbug::User user = rsp2.friendlists(i);
            std::cout << "name: " << user.name();
            std::cout << " age: " << user.age();
            std::cout << " sex: " << user.sex() << std::endl;
        }
    }

    return 0;
}