#ifndef MPRPCCONFIG_H
#define MPRPCCONFIG_H

#include <unordered_map>
#include <string>

/// @brief 框架中用于读取配置文件的类 rpcserver_ip rpcserver_port zookeeper_ip zookeeper_port
class MprcpConfig
{
private:
  std::unordered_map<std::string, std::string> m_configMap; //存储配置信息
public:
  //加载解析配置文件
  void LoadConfigFile(const char* config_file);

  //查询配置项信息
  std::string Load(const std::string& key);
};

#endif //MPRPCCONFIG_H