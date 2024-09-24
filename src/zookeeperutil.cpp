#include "zookeeperutil.h"
#include "mprpcapplication.h"

//全局的watcher观察器 zk_server给zk_client回应时会调用此函数
void global_watcher(zhandle_t *zh, int type, 
          int state, const char *path,void *watcherCtx)
{
  //在收到会话相关的消息类型时，判断连接成功，则通知连接请求的调用方
  if (type == ZOO_SESSION_EVENT) {
    if (state == ZOO_CONNECTED_STATE) {
      //获取句柄中的信号量信息，发送通知
      sem_t* sem = static_cast<sem_t*>(const_cast<void*>(zoo_get_context(zh)));
      sem_post(sem);
    }
  }
}

//构造
ZkClient::ZkClient() : m_zhandle(nullptr) {}

//析构
ZkClient::~ZkClient()
{
  if (m_zhandle != nullptr) {
    zookeeper_close(m_zhandle); //关闭句柄，释放资源
  }
}

//zookeeper_client启动连接zookeeper_server
void ZkClient::Start()
{
  //解析配置文件，获取zookeeper服务端的zookeeper_ip和zookeeper_port
  std::string zk_ip = MprpcApplication::GetConfig().Load("zookeeper_ip");
  std::string zk_port = MprpcApplication::GetConfig().Load("zookeeper_port");
  //拼接成对应格式的host参数
  std::string host = zk_ip + ":" + zk_port;

  /*
   zookeeper_mt: 多线程版本
   zookeeper的API客户端程序提供了三个线程
   1-API调用线程；2-网络I/O线程(poll模型)；3-watcher回调线程
  */
  //创建zhandle_t句柄，发起连接请求
  m_zhandle = zookeeper_init(host.c_str(), global_watcher, 30000, nullptr, nullptr, 0);
  if (m_zhandle == nullptr) {
    std::cerr << "zookeeper_init error!" << std::endl;
    exit(EXIT_FAILURE);
  }

  //为句柄添加上下文信息 信号量
  sem_t sem;
  sem_init(&sem, 0, 0);
  zoo_set_context(m_zhandle, &sem);

  //等待服务端回应调用自定义回调，回调中会向这里发送通知
  sem_wait(&sem);

  std::cout << "zookeeper_init success!" << std::endl;
}

//在zookeeper上根据指定的path完整路径节点名创建znode节点，并设置数据和状态
void ZkClient::Create(const char* path, const char* data, const int datalen, int state)
{
  //准备api方法所需参数 先判断节点是否存在，不存在则创建，避免重复创建
  char path_buffer[128]{};
  int path_buffer_len = sizeof(path_buffer);

  if (zoo_exists(m_zhandle, path, 0, nullptr) == ZNONODE) {
    //节点不存在时，创建节点
    int flag = 0; //返回状态
    if ((flag = zoo_create(m_zhandle, path, data, datalen, &ZOO_OPEN_ACL_UNSAFE, state,
      path_buffer, path_buffer_len)) == ZOK) {
      std::cout << "znode create success... path: " << path << std::endl;
    }
    else {
      std::cerr << "flags: " << flag << std::endl;
      std::cout << "znode create error... path: " << path << std::endl;
      exit(EXIT_FAILURE);
    }
  }
}

//根据path指定的完整路径节点名，获取znode节点的值
std::string ZkClient::GetData(const char* path) const
{
  char buffer[64]{};
  int buffer_len = sizeof(buffer);
  int flag = zoo_get(m_zhandle, path, 0, buffer, &buffer_len, nullptr);
  if (flag == ZOK) {
    //success
    return buffer;
  }
  //failed
  std::cout << "get znode value error... path: " << path << std::endl;
  return "";
}

