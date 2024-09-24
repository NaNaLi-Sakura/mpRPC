#include <iostream>
#include <string>
#include "user.pb.h"
#include "mprpcapplication.h"
#include "rpcprovider.h"

/// @brief 本地服务，提供进程内的两个方法(Login和GetFriendList)
class UserService : public fixbug::UserServiceRpc //使用在rpc服务发布端（rpc服务提供者）
{
public:
  //本地的Login服务
  bool Login(const std::string& name, const std::string& pwd) {
    std::cout << "doing local service : Login" << std::endl;
    std::cout << "name : " << name << ", pwd : " << pwd << std::endl;

    return true;
  }
  //本地的Regist服务
  bool Regist(const uint32_t id, const std::string& name, const std::string& pwd) {
    std::cout << "doing local service : Regist" << std::endl;
    std::cout << "id : " << id << ", name : " << name << ", pwd : " << pwd << std::endl;

    return true;
  }

  //重写基类UserServiceRpc的虚函数 下面这些方法都是框架直接调用
  //框架给业务上报了请求参数LoginRequest，程序获取相应数据做本地业务
  void rLogin(::google::protobuf::RpcController *controller,
            const ::fixbug::LoginRequest *request,
            ::fixbug::LoginResponse *response,
            ::google::protobuf::Closure *done) override
  {
    //解析传来的数据
    std::string name = request->name();
    std::string pwd = request->pwd();

    //做本地业务
    bool login_result = Login(name, pwd);

    //将返回的数据写入响应 错误码、错误信息、返回值(是否成功)
    fixbug::ResultCode* retcode = response->mutable_result();
    if (login_result) {
      retcode->set_errcode(0);
      retcode->set_errmsg("");
    }
    else {
      retcode->set_errcode(1); //错误
      retcode->set_errmsg("Login do error!");
    }
    response->set_success(login_result);

    //执行回调操作 执行响应对象数据的序列化和网络发送（由框架完成）
    done->Run();
  }
  //重写基类UserServiceRpc的rpc虚函数，实现真正的rpc方法
  void rRegist(::google::protobuf::RpcController *controller,
            const ::fixbug::RegistRequest *request,
            ::fixbug::RegistResponse *response,
            ::google::protobuf::Closure *done) override
  {
    //解析请求参数中的数据
    uint32_t id = request->id();
    std::string name = request->name();
    std::string pwd = request->pwd();

    //做本地业务
    bool regist_result = Regist(id, name, pwd);

    //将返回数据写入响应
    fixbug::ResultCode* result = response->mutable_result();
    if (regist_result) {
      result->set_errcode(0);
      result->set_errmsg("");
    }
    else {
      result->set_errcode(1);
      result->set_errmsg("Regist do error!");
    }
    response->set_success(regist_result);

    //执行回调 数据序列化、网络发送
    done->Run();
  }
};

int main(int argc, char** argv)
{
  //调用框架的初始化操作
  MprpcApplication::Init(argc, argv);

  //provider是一个rpc网络服务对象。将UserService对象发布到rpc节点上
  RpcProvider provider;
  provider.NotifyService(new UserService{});

  //启动一个rpc服务发布节点 Run以后，进程进入阻塞状态，等待远程的rpc调用请求
  provider.Run();


  return 0;
}