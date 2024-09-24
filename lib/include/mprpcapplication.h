#ifndef MPRCPAPPLICATION_H
#define MPRCPAPPLICATION_H

#include "mprpcconfig.h" //负责加载配置文件的类
#include "mprpcchannel.h" //自定义实现rpc调用方法
#include "mprpccontroller.h" //控制模块

/// @brief mprpc框架的基础类，负责框架的一些初始化操作
class MprpcApplication
{
private:
  static MprcpConfig m_config; //解析配置文件相关

  //私有构造
  MprpcApplication() {}
  MprpcApplication(const MprpcApplication&) = delete;
  MprpcApplication(MprpcApplication&&) = delete;
public:
  //获取单例对象
  static MprpcApplication& getInstance();

  //框架的初始化操作
  static void Init(int argc, char** argv);

  //获取配置文件的信息
  static MprcpConfig& GetConfig();
};

#endif //MPRCPAPPLICATION_H