#include <iostream>
#include "user.pb.h"
#include "mprpcapplication.h"
#include "mprpcchannel.h"

int main(int argc, char** argv)
{
  //程序启动后，使用mprpc框架提供的rpc服务调用，需要先调用框架的初始化函数（只初始化一次即可）
  MprpcApplication::Init(argc, argv);

  //演示调用远程发布的rpc方法"rLogin"
  //--------------------------------------------------------------------------------
  fixbug::UserServiceRpc_Stub stub(new MprpcChannel{});

  //准备rpc方法的参数
  fixbug::LoginRequest request; //请求
  request.set_name("zhang san");
  request.set_pwd("123456");
  fixbug::LoginResponse response; //响应

  //发起rpc方法的调用 同步的rpc调用过程 MprpcChannel::CallMethod
  stub.rLogin(nullptr, &request, &response, nullptr);

  //一次调用完成，读取响应结果
  if (response.result().errcode() == 0) {
    //成功
    std::cout << "rpc rLogin response success: " << response.success() << std::endl;
  }
  else {
    //失败
    std::cout << "rpc rLogin response error: " << response.result().errmsg() << std::endl;
  }

  //演示调用远程发布的rpc方法"rRegist"
  //--------------------------------------------------------------------------------
  //准备rpc请求参数 响应参数
  fixbug::RegistRequest regist_request;
  regist_request.set_id(1000);
  regist_request.set_name("li si");
  regist_request.set_pwd("234567");
  fixbug::RegistResponse regist_response;

  //复用之前的代理类，以同步的方式发起rpc方法的调用，等待返回结果
  stub.rRegist(nullptr, &regist_request, &regist_response, nullptr);

  //一次rpc调用完成，读取响应结果
  if (regist_response.result().errcode() == 0) {
    std::cout << "rpc rRegist response success: " << regist_response.success() << std::endl;
  }
  else {
    std::cout << "rpc rRegist response error: " << regist_response.result().errmsg() << std::endl;
  }


  return 0;
}