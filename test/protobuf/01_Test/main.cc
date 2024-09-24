#include "testA1.pb.h"
#include <iostream>
#include <string>

using namespace fixbug;

int main(void)
{
  //封装login请求对象的数据
  LoginRequest loginRequest;
  loginRequest.set_name("zhang san");
  loginRequest.set_pwd("123456");

  //将对象序列化为字符串、字节流
  std::string send_str;
  if (loginRequest.SerializeToString(&send_str)) {
    //输出序列化完成的字符串作演示
    std::cout << send_str << std::endl;
  }

  //从接收的字符串中解析数据，将字符串反序列化成一个对象
  LoginRequest recvLoginRequest;
  if (recvLoginRequest.ParseFromString(send_str)) {
    //输出反序列化完成的对象成员作演示
    std::cout << recvLoginRequest.name() << std::endl;
    std::cout << recvLoginRequest.pwd() << std::endl;
  }



  return 0;
}