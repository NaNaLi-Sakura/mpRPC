#ifndef RPCPROVIDER_H
#define RPCPROVIDER_H

#include "google/protobuf/service.h"
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/TcpConnection.h>
#include <muduo/net/InetAddress.h>
#include <string>
#include <unordered_map>
#include <google/protobuf/descriptor.h>

/// @brief 框架提供的用于发布rpc服务的网络对象类
class RpcProvider
{
private:
  //组合EventLoop对象
  muduo::net::EventLoop m_eventLoop;

  //service服务类型信息
  struct ServiceInfo
  {
    google::protobuf::Service* m_service; //存储服务对象
    std::unordered_map<std::string, const google::protobuf::MethodDescriptor*> m_methodMap; //方法名<=>方法描述
  };
  //存储注册成功的服务对象和其服务方法的所有信息 服务名<=>服务类型信息
  std::unordered_map<std::string, struct ServiceInfo> m_serviceMap;

  //新的socket连接回调
  void OnConnection(const muduo::net::TcpConnectionPtr&);

  //已建立连接用户的读写事件回调
  void OnMessage(const muduo::net::TcpConnectionPtr&, muduo::net::Buffer*, muduo::Timestamp);

  //Closure的回调操作，用于序列化rpc响应和网络发送的自定义方法
  void SendRpcResponse(const muduo::net::TcpConnectionPtr& conn, google::protobuf::Message* response);
  
public:
  //框架提供给外部使用，可以发布rpc方法的接口
  void NotifyService(google::protobuf::Service* service);

  //启动rpc服务节点，开始提供rpc远程网络调用服务
  void Run();

};

#endif //RPCPROVIDER_H