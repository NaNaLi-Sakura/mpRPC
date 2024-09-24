/*
 程序 : logger.h

 功能 : 日志系统的声明

 作者 : 雪与冰心丶
*/

#ifndef LOGGER_H
#define LOGGER_H

#include "lockqueue.hpp"
#include <string>

//日志信息级别
enum class LogLevel
{
  INFO, //普通信息
  ERROR, //错误信息
};

/// @brief Mprpc框架提供的日志系统
class Logger
{
private:
  unsigned int m_loglevel; //日志级别
  LockQueue<std::string> m_lockQue; //日志队列

  //私有构造
  Logger();
  //禁用一切拷贝
  Logger(const Logger&) = delete;
  Logger(Logger&&) = delete;
public:
  //获取日志的接口
  static Logger& GetInstance();
  //设置日志级别
  void SetLogLevel(const unsigned int loglevel);
  //写日志 将日志信息写入lockqueue缓冲队列中
  void Log(const std::string& message);
};

//定义宏 供用户直接使用
//普通的日志信息
#define LOG_INFO(logMsgFormat, ...) \
  do \
  { \
    Logger& logger = Logger::GetInstance(); \
    logger.SetLogLevel(static_cast<unsigned int>(LogLevel::INFO)); \
    char strmsg[1024]{}; \
    snprintf(strmsg, 1024, logMsgFormat, ##__VA_ARGS__); \
    logger.Log(strmsg); \
  } while (0);
//错误信息
#define LOG_ERR(logMsgFormat, ...) \
  do \
  { \
    Logger& logger = Logger::GetInstance(); \
    logger.SetLogLevel(static_cast<unsigned int>(LogLevel::ERROR)); \
    char strmsg[1024]{}; \
    snprintf(strmsg, 1024, logMsgFormat, ##__VA_ARGS__); \
    logger.Log(strmsg); \
  } while (0);

#endif //LOGGER_H