#include <iostream>
#include "testA1.pb.h"
#include <string>

using namespace fixbug;

//测试登录响应 复习01_Test
std::string getLoginResponse();

int main(void)
{
  //测试登录响应 复习01_Test
  // std::string send_str = getLoginResponse();
  // std::cout << send_str << std::endl;

  //创建"好友列表响应"的对象 填充 retcode 数据
  // GetFriendListResponse friendListResponse;
  // ResultCode* p_retcode = friendListResponse.mutable_retcode();
  // p_retcode->set_errcode(11); //这个int32类型是不是无法储存？还是读不出来？
  // p_retcode->set_errmsg("获取好友失败");
  // std::string send_str;
  // if (p_retcode->SerializeToString(&send_str)) {
  //   std::cout << send_str << std::endl;
  // }
  // std::cout << p_retcode->errcode() << std::endl;
  // std::cout << p_retcode->errmsg() << std::endl;

  //创建"好友列表响应"的对象 填充 friend_list 数据
  GetFriendListResponse friendListResponse;
  User* p_user1 = friendListResponse.add_friend_list();
  p_user1->set_name("zhang san");
  p_user1->set_age(20);
  p_user1->set_sex(User::MAN);
  
  User* p_user2 = friendListResponse.add_friend_list();
  p_user2->set_name("li si");
  p_user2->set_age(18);
  p_user2->set_sex(User::WOMAN);

  //好友列表个数
  std::cout << friendListResponse.friend_list_size() << std::endl;

  //获取具体的好友信息
  User user1 = friendListResponse.friend_list(0);
  std::cout << user1.name() << std::endl;
  std::cout << user1.age() << std::endl;
  std::cout << user1.sex() << std::endl;

  User* user2 = friendListResponse.mutable_friend_list(1);
  std::cout << user2->name() << std::endl;
  std::cout << user2->age() << std::endl;
  std::cout << user2->sex() << std::endl;



  return 0;
}

//测试登录响应 复习01_Test
std::string getLoginResponse()
{
  //创建"登录响应"的对象 填充消息 序列化为字符串
  LoginResponse LoginResponse;
  LoginResponse.set_errcode(11); //这个据我测试，并没有存入send_str中啊？
  LoginResponse.set_errmsg("登录失败");

  std::string send_str;
  if (LoginResponse.SerializeToString(&send_str)) {
    return send_str;
  }

  return "";
}