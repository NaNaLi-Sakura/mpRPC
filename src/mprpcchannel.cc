#include "mprpcchannel.h"
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include "rpcheader.pb.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include "mprpcapplication.h"
#include "zookeeperutil.h"

//向目标ip和端口发起socket连接
int conntodst(const char* ip, const unsigned int port, google::protobuf::RpcController* controller);

//stub代理类对象调用的rpc方法的真正实现 做rpc方法的数据序列化和网络发送
void MprpcChannel::CallMethod(const google::protobuf::MethodDescriptor* method,
        google::protobuf::RpcController* controller,
        const google::protobuf::Message* request,
        google::protobuf::Message* response,
        google::protobuf::Closure* done)
{
  //1.准备数据 header_size + header_str(service_name method_name args_size) + args_str
  //------------------------------------------------------------------------
  //方法描述=》服务描述=》服务名
  const google::protobuf::ServiceDescriptor* serviceDesc = method->service();
  std::string service_name = serviceDesc->name(); //service_name
  //方法描述=》方法名
  std::string method_name = method->name(); //method_name

  //将request参数序列化成字符串=》args_str、args_size
  uint32_t args_size = 0;
  std::string args_str{};
  if (request->SerializeToString(&args_str)) {
    //序列化成功，得到参数字符串大小
    args_size = args_str.size(); //args_size
  }
  else {
    controller->SetFailed("serialize request error!");
    return;
  }

  //填充protobuf数据头RpcHeader对象，序列化=》header_str、header_size
  mprpc::RpcHeader rpcHeader;
  rpcHeader.set_service_name(service_name);
  rpcHeader.set_method_name(method_name);
  rpcHeader.set_args_size(args_size);
  uint32_t header_size = 0;
  std::string rpc_header_str{};
  if (rpcHeader.SerializeToString(&rpc_header_str)) {
    //序列化成功，得到数据头字符串大小
    header_size = rpc_header_str.size(); //header_size
  }
  else {
    controller->SetFailed("serialize rpc header error!");
    return;
  }

  //组合待发送的rpc请求数据字符串 header_size(二进制 4字节存储) + header_str + args_str
  std::string rpc_send_str{};
  rpc_send_str.insert(0, std::string(reinterpret_cast<char*>(&header_size), 4));
  // rpc_send_str += header_str; rpc_send_str += args_str;
  rpc_send_str.append(rpc_header_str).append(args_str);
//调试
std::cout << "rpc_send_str: " << rpc_send_str << std::endl;

  //调试
  std::cout << "==========================================================" << std::endl;
  std::cout << "header_size: " << header_size << std::endl;
  std::cout << "rpc_header_str: " << rpc_header_str << std::endl;
  std::cout << "service_name: " << service_name << std::endl;
  std::cout << "method_name: " << method_name << std::endl;
  std::cout << "args_size: " << args_size << std::endl;
  std::cout << "args_str: " << args_str << std::endl;
  std::cout << "==========================================================" << std::endl;

  //2.发送数据，阻塞等待，获取数据，反序列化写入响应
  //------------------------------------------------------------------------
  //读取配置文件获取rpcserver的信息
  // std::string rpcserver_ip = MprpcApplication::getInstance().GetConfig().Load("rpcserver_ip");
  // uint16_t rpcserver_port = atoi(MprpcApplication::getInstance().GetConfig().Load("rpcserver_port").c_str());

  //rpc调用方想要调用service_name服务中的method_name方法，需要先从zk注册中心上查询到该服务的host信息，才可连接到对应的服务提供方
  //------------------------------------------------------------------------
  //连接zk_server，拼接方法所在的节点路径，获取数据，解析出ip和port
  ZkClient zkCli;
  zkCli.Start();

  std::string method_path = "/" + service_name + "/" + method_name;
  std::string host_data = zkCli.GetData(method_path.c_str());
  if (host_data == "") {
    //获取数据失败 设置状态信息，返回
    controller->SetFailed(method_path + " GetData error!");
    return;
  }
  //解析ip和port
  size_t index = host_data.find(":");
  if (index == std::string::npos) {
    //ip和port数据不正确
    controller->SetFailed(method_path + " address is invalid!");
    return;
  }
  std::string rpcserver_ip = host_data.substr(0, index);
  uint16_t rpcserver_port = atoi(host_data.substr(index + 1, host_data.length() - index - 1).c_str());
  //------------------------------------------------------------------------

  //向目标ip和端口发起socket连接
  int clientsock = conntodst(rpcserver_ip.c_str(), rpcserver_port, controller);
  if (clientsock == -1) return;

  //发送rpc请求数据，完成rpc方法的远程调用
  if (send(clientsock, rpc_send_str.data(), rpc_send_str.size(), 0) == -1) {
    char errText[512]{};
    sprintf(errText, "send error! errno: %d", errno);
    controller->SetFailed(errText);
    close(clientsock); return;
  }
  
  //阻塞等待接收rpc请求的响应数据
  char recv_buf[1024]{};
  int recv_size = recv(clientsock, recv_buf, sizeof(recv_buf), 0);
  if (recv_size == -1) {
    char errText[512]{};
    sprintf(errText, "recv error! errno: %d", errno);
    controller->SetFailed(errText);
    close(clientsock); return;
  }

  //反序列化响应数据，写入响应供主程序读取
  // if (!response->ParseFromArray(recv_buf, recv_size)) //OK!
  std::string response_str(recv_buf, recv_size);
  if (!response->ParseFromString(response_str)) {
    char errText[512]{};
    sprintf(errText, "parse error! response_str: %s", response_str.c_str());
    controller->SetFailed(errText);
    close(clientsock); return;
  }

  //关闭socket
  close(clientsock); clientsock = -1;
}

//向目标ip和端口发起socket连接
int conntodst(const char* ip, const unsigned int port, google::protobuf::RpcController* controller)
{
  //创建，连接请求。
  int clientsock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (clientsock == -1) {
    char errText[512]{};
    sprintf(errText, "create socket error! errno: %d", errno);
    controller->SetFailed(errText);
    return -1;
  }

  struct sockaddr_in servaddr{};
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(port);
  struct hostent* h = gethostbyname(ip);
  if (h == nullptr) { close(clientsock); return -1; }
  memcpy(&servaddr.sin_addr, h->h_addr, h->h_length);
  if (connect(clientsock, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1) {
    char errText[512]{};
    sprintf(errText, "connect error! errno: %d", errno);
    controller->SetFailed(errText);
    close(clientsock); return -1;
  }

  return clientsock;
}