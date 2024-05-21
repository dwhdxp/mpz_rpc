# mprpc

## 项目介绍

项目是在Linux环境下基于muduo网络库、protobuf、Zookeeper实现的高性能rpc分布式通信框架，通过该框架可以实现不同分布式节点之间远程方法的调用以及同一节点不同进程之间方法的调用。该通信框架主要涉及两部分内容，一是使用protobuf将rpc请求调用的方法及参数序列化，再将rpc响应结果反序列化；二是使用Zookeeper和muduo实现服务注册发现功能和网络通信；



**RPC框架中核心类的介绍：**

Callee：

* MpzrpcApplication类（rpc框架基础类）：进行RPC框架初始化操作，解析配置文件，从而将RPC框架部署到该节点上；
* RpcProvider类（框架提供的服务Rpc服务类）：
	* `NotifyService()`：提供发布RPC方法（ServiceInfo：服务类及类内发布的所有rpc方法）的接口，主要负责记录服务对象和其发布的所有rpc方法，当接收请求时，RPC框架可根据记录定位并调用某个服务对象的某个rpc方法，例如UserService的Login()方法；
	* `Run()`：启动RPC服务节点，提供rpc远程网络调用服务，主要负责rpc请求的接收并反序列化上报给RPC节点，同时会把响应序列化后通过网络发送；

Caller：

* MpzrpcChannel类（继承RpcChannel）：protoc生成的Stub类，将所有的调用都指向RpcChannel中的CallMethod()方法，但CallMethod是一个纯虚函数，因此需要重写并实现CallMethod()，从而通过基类指针来完成基于protobuf的RPC调用，而不是将调用限定在一个特定的RPC实现中；
	* `CallMethod()`：负责将rpc请求序列化并通过网络发送，同时接收响应并反序列化；



**RPC框架调用流程：**

* Callee（Provider）
	* NotifyService()发布服务对象和rpc方法到rpc服务节点，并记录service类和其上发布的所有rpc方法；
	* Run()启动RPC服务节点，提供rpc远程网络调用服务；
		* 基于muduo的网络层，设置4个线程，1个I/O线程，3个工作线程；
		* 将RPC节点上发布的rpc服务注册到ZooKeeper上；
	* 当有client发来rpc请求时，会调用OnMessage()方法处理：
		* 按消息格式解析rpc请求，将请求反序列化；
		* RPC框架根据请求定位并调用某RPC服务节点上的对应的rpc方法；
	* RPC节点执行rpc方法：
		* 根据rpc框架上报的request做本地业务；
		* 执行回调操作，将response序列化后发送给client，然后关闭连接；
* Caller（Consumer）
	* 实现一个继承自RpcChannel的类，并重写CallMethod()方法，构造ServiceRpc_Stub对象，并通过Stub对象调用相应rpc方法；
		* 按照协商的消息格式组装rpc请求，并基于protobuf进行序列化；
		* 访问ZooKeeper查询rpc服务对应分布式节点的ip和port；
		* 基于tcp网络编程，将rpc请求发送；
		* 接收响应，并反序列化response返回给client；

 ## example

在业务层，服务提供方将本地服务发布成rpc远程服务流程：

* 在.proto文件中定义request、response的message类型，然后添加希望提供的服务service类和其中的rpc方法，通过protoc生成描述rpc方法的服务类ServiceRpc；
* 服务提供方定义继承ServiceRpc类，并重写基类ServiceRpc中的虚函数，即.proto中定义的rpc方法；
	* 获取rpc框架上报的request；
	* 使用request中的参数做本地业务；
	* 将返回值写入response数据；
	* 执行回调操作，将响应数据序列化并网络发送给client；



## 日志系统

项目实现的日志模块采用异步工作方式，其中工作线程将日志信息写入队列，然后一个专门的日志线程从队列中读取日志信息并写入磁盘中的日志文件，要点包括：1. 保证多线程操作日志队列时的线程安全；2. 当日志队列为空时，日志线程不该持有锁，因此在工作线程和日志线程也需要通过条件变量进行协作；



## Zookeeper分布式协调服务

Zookeeper是一个开源的分布式协调服务，在本项目中主要作为配置中心，提供服务动态注册和发现的功能，首先将每个rpc服务和对应的RPC节点地址注册到ZK上，此时ZK上面标识了每个rpc服务所对应的分布式节点地址；当进行rpc调用时，服务调用方只需要给出rpc服务名，注册中心就会返回一个对应的RPC节点地址，这样可以保证每次获得的都是最新的分布式节点地址。
