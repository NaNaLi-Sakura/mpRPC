/*
 程序 : mprpccontroller.cpp

 功能 : RpcController控制模块的实现

 作者 : 雪与冰心丶
*/

#include "mprpccontroller.h"

//构造
MprpcController::MprpcController() : m_failed(false), m_errText("")
{}

//重置状态
void MprpcController::Reset()
{
  m_failed = false;
  m_errText = "";
}
//获取失败状态
bool MprpcController::Failed() const
{
  return m_failed;
}
//获取失败信息
std::string MprpcController::ErrorText() const
{
  return m_errText;
}
//设置失败状态和信息
void MprpcController::SetFailed(const std::string& reason)
{
  m_failed = true;
  m_errText = reason;
}

//目前未实现具体的功能
void MprpcController::StartCancel() {}
bool MprpcController::IsCanceled() const { return false; }
void MprpcController::NotifyOnCancel(google::protobuf::Closure* callback) {}
