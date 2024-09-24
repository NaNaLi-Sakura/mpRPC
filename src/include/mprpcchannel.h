#ifndef MPRPCCHANNEL_H
#define MPRPCCHANNEL_H

#include <google/protobuf/service.h>

class MprpcChannel : public google::protobuf::RpcChannel
{
public:
  //stub代理类对象调用的rpc方法的真正实现 做rpc方法的数据序列化和网络发送
  void CallMethod(const google::protobuf::MethodDescriptor* method,
          google::protobuf::RpcController* controller,
          const google::protobuf::Message* request,
          google::protobuf::Message* response,
          google::protobuf::Closure* done) override;
};

#endif //MPRPCCHANNEL_H