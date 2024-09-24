#include "mprpcapplication.h"
#include <iostream>
#include <unistd.h>
#include <string>

//初始化静态成员变量
MprcpConfig MprpcApplication::m_config;

//获取单例对象
MprpcApplication& MprpcApplication::getInstance()
{
  static MprpcApplication app;
  return app;
}

//帮助文档
void ShowArgsHelp()
{
  std::cout << "Format:  command -i <configfile>" << std::endl;
  std::cout << "Example: /home/violet/D/PApplication/mprpc/bin/provider -i /home/violet/D/PApplication/mprpc/bin/test.conf" << std::endl;
  std::cout << "         ./bin/provider -i ./bin/test.conf" << std::endl;
  std::cout << "Example: /home/violet/D/PApplication/mprpc/bin/consumer -i /home/violet/D/PApplication/mprpc/bin/test.conf" << std::endl;
  std::cout << "         ./bin/consumer -i ./bin/test.conf" << std::endl;
  std::cout << std::endl;
}

//框架的初始化操作
void MprpcApplication::Init(int argc, char** argv)
{
  //参数检测
  if (argc < 2) {
    ShowArgsHelp();
    exit(EXIT_FAILURE);
  }

  //分析命令行参数
  int o = 0;
  std::string config_file{}; //接收配置文件
  while ((o = getopt(argc, argv, "i:")) != -1) {
    switch (o) {
    case 'i':
      config_file = optarg; //得到配置文件 <configfile>
      break;
    case '?':
      std::cout << "invalid args: " << static_cast<char>(optopt) << std::endl;
      ShowArgsHelp();
      exit(EXIT_FAILURE);
    case ':':
      std::cout << "missing argument! need <configfile>!" << std::endl;
      ShowArgsHelp();
      exit(EXIT_FAILURE);
    default:
      break;
    }
  }

  //加载配置文件 rpcserver_ip rpcserver_port zookeeper_ip zookeeper_port
  m_config.LoadConfigFile(config_file.c_str());

  //调试 配置文件加载结果
  // std::cout << "rpcserver_ip : " << m_config.Load("rpcserver_ip") << std::endl;
  // std::cout << "rpcserver_port : " << m_config.Load("rpcserver_port") << std::endl;
  // std::cout << "zookeeper_ip : " << m_config.Load("zookeeper_ip") << std::endl;
  // std::cout << "zookeeper_port : " << m_config.Load("zookeeper_port") << std::endl;
}

//获取配置文件的信息
MprcpConfig& MprpcApplication::GetConfig()
{
  return m_config;
}
