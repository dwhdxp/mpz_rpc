#include "rpcprovider.h"

void RpcProvider::NotifyService(google::protobuf::Service *service) {
    ServiceInfo service_info;
    service_info.m_service = service;
    // 获取服务对象的描述信息
    const google::protobuf::ServiceDescriptor* pserviceDesc = service->GetDescriptor(); 
    // 获取服务名
    std::string service_name = pserviceDesc->name();

    // 获取服务对象service的rpc方法数量
    int methodCnt = pserviceDesc->method_count();
    std::cout << "service_name: " << service_name << std::endl;
    
    for (int i = 0; i < methodCnt; ++i) {
        // 获取rpc方法的描述信息
        const google::protobuf::MethodDescriptor* pmethodDesc = pserviceDesc->method(i);
        std::string method_name = pmethodDesc->name();
        std::cout << "method_name: " << method_name << std::endl;
        service_info.m_methodMap.insert({method_name, pmethodDesc});
    }

    m_serviceMap.insert({service_name, service_info});
} 
    
void RpcProvider::Run() {
    // 获取rpcserver的ip和port，初始化网络层
    std::string ip = MpzrpcApplication::GetInstance().GetConfig().Load("rpcserverip");
    uint16_t port = atoi(MpzrpcApplication::GetInstance().GetConfig().Load("rpcserverport").c_str());
    muduo::net::InetAddress address(ip, port);

    // 创建TcpServer对象
    muduo::net::TcpServer server(&m_eventLoop, address, "RpcProvidder");

    // 注册连接事件和读写事件回调
    server.setConnectionCallback(std::bind(&RpcProvider::OnConnection, this, std::placeholders::_1));
    server.setMessageCallback(std::bind(&RpcProvider::OnMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    // 设置muduo线程数量
    server.setThreadNum(4);

    server.start();
    std::cout << "RpcProvider start at ip: " << ip << " port: " << port << std::endl;

    // 进程阻塞，event_wait
    m_eventLoop.loop();
}

// 新用户的连接事件
void RpcProvider::OnConnection(const muduo::net::TcpConnectionPtr& conn) {
    if (!conn->connected()) {
        // rpc连接断开，关闭fd
        conn->shutdown();
    }
}

// 已建立连接用户的读写事件：1. 解析rpc请求：请求消息的反序列化； 2. 根据rpc请求定位某service的rpc方法；3. 调用rpc方法
/*
    RpcProvider、RpcConsumer协商消息格式
    消息格式：head_size(4字节) + head_str(service_name + method_name + args_size) + args_str
*/
void RpcProvider::OnMessage(const muduo::net::TcpConnectionPtr& conn, muduo::net::Buffer* buffer, muduo::Timestamp) {
    // 1. 解析rpc，并通过反序列化获取RpcHeader中的数据
    std::string recv_buf = buffer->retrieveAllAsString();
    
    // 读取前4个字节，获取head_str长度
    uint32_t header_size = 0;
    recv_buf.copy((char*)&header_size, 4, 0);

    // 反序列化，获取rpc_header_str和rpc_args_str
    std::string rpc_header_str = recv_buf.substr(4, header_size);
    mpzrpc::RpcHeader rpcHeader;
    std::string service_name;
    std::string method_name;
    uint32_t args_size;
    if (!rpcHeader.ParseFromString(rpc_header_str)) {
        std::cout << "parse rpc_header_str error!" << std::endl;
        return;
    }
    else {
        service_name = rpcHeader.service_name();
        method_name = rpcHeader.method_name();
        args_size = rpcHeader.args_size();
    }
    std::string rpc_args_str = recv_buf.substr(4 + header_size, args_size);

    std::cout << "=========================================" << std::endl;
    std::cout << "header_size：" << header_size << std::endl;
    std::cout << "rpc_header_str：" << rpc_header_str << std::endl;
    std::cout << "service_name：" << service_name << std::endl;
    std::cout << "method_name：" << method_name << std::endl;
    std::cout << "args_size：" << args_size << std::endl;
    std::cout << "rpc_args_str：" << rpc_args_str << std::endl;
    std::cout << "=========================================" << std::endl;


    // 2. 定位rpc方法
    auto sit = m_serviceMap.find(service_name);
    if (sit == m_serviceMap.end()) {
        std::cout << "Service is not exit" << std::endl;
        return;
    }
    auto mit = sit->second.m_methodMap.find(method_name);
    if (mit == sit->second.m_methodMap.end()) {
        std::cout << "Method is not exit" << std::endl;
        return;
    }
    google::protobuf::Service *service = sit->second.m_service;
    const google::protobuf::MethodDescriptor* method = mit->second;

    // 3. 调用method
    /*
    void UserServiceRpc::CallMethod(const ::PROTOBUF_NAMESPACE_ID::MethodDescriptor* method,
                             ::PROTOBUF_NAMESPACE_ID::RpcController* controller,
                             const ::PROTOBUF_NAMESPACE_ID::Message* request,
                             ::PROTOBUF_NAMESPACE_ID::Message* response,
                             ::google::protobuf::Closure* done)
    */
    // 生成rpc方法调用的request和response参数，Loginrequest和Loginresponse都继承了Message因此使用基类指针
    google::protobuf::Message* request = service->GetRequestPrototype(method).New();
    if (!request->ParseFromString(rpc_args_str)) {
        std::cout << "request parse error, content:" << rpc_args_str << std::endl;
    }

    google::protobuf::Message* response = service->GetResponsePrototype(method).New();

    // 绑定rpc方法调用后的回调操作done->Run()
    google::protobuf::Closure *done = google::protobuf::NewCallback<RpcProvider, 
                                                                    const muduo::net::TcpConnectionPtr&, 
                                                                    google::protobuf::Message*>
                                                                    (this, 
                                                                    &RpcProvider::SendRpcResponse,
                                                                    conn,
                                                                    response);
    
    service->CallMethod(method, nullptr, request, response, done);
}

// Closure回调操作done->Run()，用于response序列化和网络发送
void RpcProvider::SendRpcResponse(const muduo::net::TcpConnectionPtr& conn, google::protobuf::Message* response) {
    std::string rpc_response;
    if (response->SerializeToString(&rpc_response)) {
        conn->send(rpc_response);
    }
    else {
        std::cout << "Response Serialize error" << std::endl;
    }
    // rpc短链接，rpcProvider主动断开连接
    conn->shutdown();
}