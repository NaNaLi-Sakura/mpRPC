#include "friend.pb.h"
#include "mprpcapplication.h"

int main(int argc, char** argv)
{
  //框架初始化
  MprpcApplication::Init(argc, argv);

  //演示调用远程发布的rpc方法"rGetFriendList"
  //--------------------------------------------------------------------------------
  fixbug::FriendServiceRpc_Stub stub(new MprpcChannel{});

  //准备rpc方法的参数
  fixbug::GetFriendListRequest request; //请求
  request.set_userid(200);
  fixbug::GetFriendListResponse response; //响应

  //以同步的方式发起rpc方法的调用，等待返回结果
  MprpcController controller;
  stub.rGetFriendList(&controller, &request, &response, nullptr);

  //一次rpc调用完成，判断调用状态
  if (controller.Failed()) {
    //调用失败，直接返回
    std::cout << controller.ErrorText() << std::endl;
    return -1;
  }
  //调用成功，读取响应结果
  if (response.result().errcode() == 0) {
    std::cout << "rpc rGetFriendList response success!" << std::endl;
    int size = response.friends_size();
    for (int i = 0; i < size; ++i) {
      std::cout << "friends[" << i << "]: " << response.friends(i) << std::endl;
    }
  }
  else {
    std::cout << "rpc rGetFriendList response error: " << response.result().errmsg() << std::endl;
  }


  return 0;
}