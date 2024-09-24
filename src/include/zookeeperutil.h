#ifndef ZOOKEEPERUTIL_H
#define ZOOKEEPERUTIL_H

#include <zookeeper/zookeeper.h>
#include <string>
#include <semaphore.h>

/// @brief 封装的zookeeper客户端类
class ZkClient
{
private:
  zhandle_t* m_zhandle; //zookeeper的客户端句柄
public:
  //构造
  ZkClient();
  //析构
  ~ZkClient();

  //zookeeper_client启动连接zookeeper_server
  void Start();

  //在zookeeper上根据指定的path完整路径节点名创建znode节点，并设置数据和状态
  void Create(const char* path, const char* data, const int datalen, int state = 0);

  //根据path指定的完整路径节点名，获取znode节点的值
  std::string GetData(const char* path) const;
};

#endif //ZOOKEEPERUTIL_H