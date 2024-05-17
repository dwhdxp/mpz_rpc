#include "mpzrpcchannel.h"

void MpzrpcChannel::CallMethod(const google::protobuf::MethodDescriptor *method,
                               google::protobuf::RpcController *controller,
                               const google::protobuf::Message *request,
                               google::protobuf::Message *response,
                               google::protobuf::Closure *done) {
    // 1. 序列化request参数，并按head_size + head_str(service_name,method_name,args_size) + args_str组装rpc请求
    const google::protobuf::ServiceDescriptor* serviceDesc = method->service();
    std::string service_name = serviceDesc->name();
    std::string method_name = method->name();

    uint32_t args_size = 0;
    std::string args_str;
    if (request->SerializeToString(&args_str)) {
        args_size = args_str.size();
    }
    else {
        std::cout << "request serialize error" << std::endl;
        return;
    }

    // 序列化rpcHeader得到head_str
    std::string header_str;
    uint32_t header_size = 0;

    mpzrpc::RpcHeader rpcHeader;
    rpcHeader.set_service_name(service_name);
    rpcHeader.set_method_name(method_name);
    rpcHeader.set_args_size(args_size);

    if (rpcHeader.SerializeToString(&header_str)) {
        header_size = header_str.size();
    }
    else {
        std::cout << "rpcHeader serialize error" << std::endl;
        return;
    }

    // 组装消息
    std::string send_rpc_str;
    send_rpc_str.insert(0, (char*)&header_size, 4); // 加&确保插入header_size变量的二进制表示，而不是header_size的值本身
    send_rpc_str += header_str;
    send_rpc_str += args_str;

    std::cout << "================================" << std::endl;
    std::cout << "header_size：" << header_size << std::endl; 
    std::cout << "header_str：" << header_str << std::endl; 
    std::cout << "service_name: " << service_name << std::endl; 
    std::cout << "method_name: " << method_name << std::endl; 
    std::cout << "args_size：" << args_size << std::endl; 
    std::cout << "args_str：" << args_str << std::endl; 
    std::cout << "================================" << std::endl;

    // 2. tcp网络编程发送消息
    int clientfd = socket(AF_INET, SOCK_STREAM, 0);
    if (clientfd == -1) {
        std::cout << "open clientfd error" << std::endl;
    }

    std::string ip = MpzrpcApplication::GetInstance().GetConfig().Load("rpcserverip");
    uint16_t port = atoi(MpzrpcApplication::GetInstance().GetConfig().Load("rpcserverport").c_str());

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip.c_str());
    // 建立连接
    if (connect(clientfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        std::cout << "connect error" << std::endl;
        close(clientfd);
        return;
    }

    // 发送rpc请求
    if (send(clientfd, send_rpc_str.c_str(), send_rpc_str.size(), 0) == -1) {
        std::cout << "send error" << std::endl;
        close(clientfd);
        return;
    }

    // 接收rpc响应
    char recv_buf[1024] {0};
    int recv_size = 0;
    if (-1 == (recv_size  = recv(clientfd, recv_buf, 1024, 0))) {
        std::cout << "recv error" << std::endl;
        close(clientfd);
        return;
    }

    // 3. 反序列化response
    if (!response->ParseFromArray(recv_buf, recv_size)) {
        std::cout << "parse recv_str error" << std::endl;
        close(clientfd);
        return;
    }

    close(clientfd);
}