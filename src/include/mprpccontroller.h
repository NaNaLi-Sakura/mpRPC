/*
 程序 : mprpccontroller.h

 功能 : RpcController控制模块的声明

 作者 : 雪与冰心丶
*/

#ifndef MPRPCCONTROLLER_H
#define MPRPCCONTROLLER_H

#include <google/protobuf/service.h>
#include <string>

class MprpcController : public google::protobuf::RpcController
{
private:
  bool m_failed; //rpc方法执行过程中的状态 是否失败
  std::string m_errText; //rpc方法执行过程中的错误信息
public:
  //构造
  MprpcController();
  //析构
  ~MprpcController() = default;

  //重置状态
  void Reset() override;
  //获取失败状态
  bool Failed() const override;
  //获取失败信息
  std::string ErrorText() const override;
  //设置失败状态和信息
  void SetFailed(const std::string& reason) override;

  //目前未实现具体的功能
  void StartCancel() override;
  bool IsCanceled() const override;
  void NotifyOnCancel(google::protobuf::Closure* callback) override;
};

#endif //MPRPCCONTROLLER_H