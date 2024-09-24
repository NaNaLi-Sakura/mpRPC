#include "rpcprovider.h"
#include "mprpcapplication.h"
#include <functional>
#include "rpcheader.pb.h"
#include "logger.h"
#include "zookeeperutil.h"

//框架提供给外部使用，可以发布rpc方法的接口
void RpcProvider::NotifyService(google::protobuf::Service* service)
{
  struct ServiceInfo service_info{}; //用于存储服务类型信息

  //获取服务对象的描述信息
  const google::protobuf::ServiceDescriptor* pserviceDesc = service->GetDescriptor();
  //获取服务的名字
  std::string service_name = pserviceDesc->name();
  // std::cout << "service_name: " << service_name << std::endl;
  LOG_INFO("service_name: %s", service_name.c_str());
  //获取服务对象service的方法数量
  int method_count = pserviceDesc->method_count();
  //获取服务对象各个服务方法对象的描述信息(抽象的服务方法描述)，同时获取方法名字，存储到结构体成员。
  for (int i = 0; i < method_count; ++i) {
    //获取方法描述
    const google::protobuf::MethodDescriptor* pmethodDesc = pserviceDesc->method(i);
    //获取方法名字
    std::string method_name = pmethodDesc->name();
    // std::cout << "method_name[" << i << "]: " << method_name << std::endl;
    LOG_INFO("method_name[%d]: %s", i, method_name.c_str());
    
    //存储"方法名<=>方法描述"
    service_info.m_methodMap.insert({method_name, pmethodDesc});
  }

  //存储"服务对象"
  service_info.m_service = service;
  //存储"服务名<=>服务类型"
  m_serviceMap.insert({service_name, service_info});
}

//启动rpc服务节点，开始提供rpc远程网络调用服务
void RpcProvider::Run()
{
  //从配置文件中加载ip和port 组合TcpServer对象
  std::string ip = MprpcApplication::getInstance().GetConfig().Load("rpcserver_ip");
  uint16_t port = atoi(MprpcApplication::getInstance().GetConfig().Load("rpcserver_port").c_str());
  muduo::net::InetAddress listenAddr(ip, port);
  muduo::net::TcpServer server(&m_eventLoop, listenAddr, "RpcProvider");

  //绑定连接回调和消息读写回调方法 分离网络代码和业务代码
  server.setConnectionCallback(std::bind(&RpcProvider::OnConnection, this, std::placeholders::_1));
  server.setMessageCallback(std::bind(&RpcProvider::OnMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

  //设置muduo库的线程数量
  server.setThreadNum(4);

  //将当前rpc节点上要发布的服务全部注册到zk上，使rpc client可以从zk上发现服务的提供方
  //------------------------------------------------------------------------------------
  ZkClient zkCli;
  zkCli.Start(); //连接zk_server

  //service_name和method_name作为节点的名称
  //service_name是永久性节点，method_name是临时性节点
  //rpc服务提供方的ip和port作为数据存储到对应节点
  //service_name是主节点，不存放数据，method_name是子节点，存放数据

  //遍历存储的服务信息 拼接节点路径 逐层创建节点并添加数据及设置状态l
  std::string service_path{}; std::string method_path{};
  for (auto& kv : m_serviceMap) {
    //创建主节点
    service_path = "/" + kv.first;
    zkCli.Create(service_path.c_str(), nullptr, 0, 0);
    //创建各子节点 并添加数据设置状态
    for (auto& kv : kv.second.m_methodMap) {
      method_path = service_path + "/" + kv.first;
      char method_path_data[128]{};
      sprintf(method_path_data, "%s:%d", ip.c_str(), port);
      zkCli.Create(method_path.c_str(), method_path_data, strlen(method_path_data), ZOO_EPHEMERAL);
    }
  }
  //------------------------------------------------------------------------------------

  //启动网络服务 等待rpc调用请求
  // std::cout << "RpcProvider start service at " << ip << ":" << port << std::endl;
  LOG_INFO("RpcProvider start service at %s:%d", ip.c_str(), port);
  server.start();
  m_eventLoop.loop();
}

//新的socket连接回调
void RpcProvider::OnConnection(const muduo::net::TcpConnectionPtr& conn)
{
  if (!conn->connected()) {
    //rpc client的连接断开，则关闭socket。
    conn->shutdown();
  }
}

//已建立连接用户的读写事件回调 当远程有rpc服务的调用请求过来，此方法就会响应。
void RpcProvider::OnMessage(const muduo::net::TcpConnectionPtr& conn, muduo::net::Buffer* buffer, muduo::Timestamp time)
{
  //1.接收rpc调用请求的字节流，解析得到服务名、方法名、参数等信息
  //------------------------------------------------------------------------------------
  //接收远程rpc调用请求的数据字节流，将其转换为字符串 header_size(4字节) + header_str + args_str
  std::string recv_buf = buffer->retrieveAllAsString();
//调试
std::cout << "recv_buf: " << recv_buf << std::endl;
  
  //接收数据头大小 copy
  uint32_t header_size = 0;
  recv_buf.copy(reinterpret_cast<char*>(&header_size), 4, 0);
//调试
std::cout << "header_size: " << header_size << std::endl;

  //接收数据头数据 substr
  std::string rpc_header_str = recv_buf.substr(4, header_size);
  //与调用方规定好protobuf类型，将数据头字符串反序列化为protobuf对象，解析数据得到rpc请求中的service_name和method_name
  mprpc::RpcHeader rpcHeader;
  std::string service_name{};
  std::string method_name{};
  uint32_t args_size = 0;
  if (rpcHeader.ParseFromString(rpc_header_str)) {
    //数据头反序列化成功，解析数据得到service_name和method_name
    service_name = rpcHeader.service_name();
    method_name = rpcHeader.method_name();
    args_size = rpcHeader.args_size();
  }
  else {
    //数据头反序列化失败
    std::cout << "rpc_header_str: " << rpc_header_str << " parse error!" << std::endl;
    return;
  }

  //接收rpc方法参数的字符流数据 substr
  std::string args_str = recv_buf.substr(4 + header_size, args_size);

  //调试
  std::cout << "==========================================================" << std::endl;
  std::cout << "header_size: " << header_size << std::endl;
  std::cout << "rpc_header_str: " << rpc_header_str << std::endl;
  std::cout << "service_name: " << service_name << std::endl;
  std::cout << "method_name: " << method_name << std::endl;
  std::cout << "args_size: " << args_size << std::endl;
  std::cout << "args_str: " << args_str << std::endl;
  std::cout << "==========================================================" << std::endl;

  //2.获取服务对象、方法对象，填充调用rpc方法的各种参数，调用rpc方法
  //------------------------------------------------------------------------------------
  //根据service_name和method_name，获取service对象和method对象描述
  struct ServiceInfo serviceInfo{}; //服务类型信息
  try {
    serviceInfo = m_serviceMap.at(service_name);
  }
  catch (const std::out_of_range& exception) {
    std::cout << service_name << " is not exists!" << std::endl;
    return;
  }
  const google::protobuf::MethodDescriptor* methodDesc{}; //方法对象描述 rLogin
  try {
    methodDesc = serviceInfo.m_methodMap.at(method_name);
  }
  catch (const std::out_of_range&) {
    std::cout << service_name << ":" << method_name << " is not exists!" << std::endl;
    return;
  }
  google::protobuf::Service* service = serviceInfo.m_service; //服务对象 new UserService

  //生成protobuf对象类型的请求request和响应response参数
  google::protobuf::Message* request = service->GetRequestPrototype(methodDesc).New();
  google::protobuf::Message* response = service->GetResponsePrototype(methodDesc).New();
  //将请求参数args_str反序列化填充到request 响应参数会在rpc方法内部得到响应数据集后填充到response
  if (!request->ParseFromString(args_str)) {
    std::cout << "request parse error! args_str: " << args_str << std::endl;
    return;
  }
  //为填充google::protobuf::Closure* done参数，绑定一个Closure的回调函数，给method方法调用
  google::protobuf::Closure *done = google::protobuf::NewCallback<RpcProvider,
                            const muduo::net::TcpConnectionPtr&,
                            google::protobuf::Message*>
                            (this, &RpcProvider::SendRpcResponse,
                            conn, response);

  //调用rcp方法 (在框架上根据远端rpc请求，调用当前rpc节点上发布的方法)
  //new UserService().rLogin(controller, request, response, done);
  service->CallMethod(methodDesc, nullptr, request, response, done);
}

//Closure的回调操作，用于序列化rpc响应和网络发送的自定义方法
void RpcProvider::SendRpcResponse(const muduo::net::TcpConnectionPtr& conn, google::protobuf::Message* response)
{
  //将响应数据(protobuf对象)序列化为字符串字节流，发送到网络 断开连接
  std::string response_str{};
  if (response->SerializeToString(&response_str)) {
    //序列化成功，将响应字符串发送给对端的rpc调用方
    conn->send(response_str);
  }
  else {
    std::cout << "serialize response_str error!" << std::endl;
  }
  conn->shutdown(); //模拟http的短连接服务，由rpcprovider主动断开连接
}
