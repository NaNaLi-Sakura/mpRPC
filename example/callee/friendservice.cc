#include "friend.pb.h"
#include <vector>
#include <string>
#include <iostream>
#include "mprpcapplication.h"
#include "rpcprovider.h"
#include "logger.h"

/// @brief 本地服务 好友模块
class FriendService : public fixbug::FriendServiceRpc
{
public:
  //本地的GetFriendList服务
  std::vector<std::string> GetFriendList(const uint32_t userid) {
    std::cout << "start local service: GetFriendList! userid: " << userid << std::endl;
    std::vector<std::string> vresult;
    vresult.push_back("name1");
    vresult.push_back("name2");
    vresult.push_back("name3");
    vresult.push_back("name4");

    return vresult;
  }

  //重写基类FriendServiceRpc的虚函数，实现真正的rpc方法
  void rGetFriendList(::google::protobuf::RpcController* controller,
              const ::fixbug::GetFriendListRequest* request,
              ::fixbug::GetFriendListResponse* response,
              ::google::protobuf::Closure* done) override
  {
    //解析请求参数中的数据
    uint32_t userid = request->userid();

    //做本地业务
    std::vector<std::string> vfriend = GetFriendList(userid);

    //将返回数据写入响应
    response->mutable_result()->set_errcode(0);
    response->mutable_result()->set_errmsg("");
    for (std::string& friend_name : vfriend) {
      // std::string* p_friend = response->add_friends();
      // *p_friend = friend_name;
      *(response->add_friends()) = friend_name;
    }

    //执行回调 数据序列化、网络发送
    done->Run();
  }
};

int main(int argc, char** argv)
{
  //调用框架的初始化操作
  MprpcApplication::Init(argc, argv);

  //借用rpc网络服务对象，将FriendService对象发布到rpc节点上
  RpcProvider provider;
  provider.NotifyService(new FriendService{});

  //启动一个rpc服务发布节点 阻塞等待远程的rpc调用请求
  provider.Run();


  return 0;
}