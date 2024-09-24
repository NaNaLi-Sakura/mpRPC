/*
 程序 : logger.cpp

 功能 : 日志系统的实现

 作者 : 雪与冰心丶
*/

#include "logger.h"
#include <time.h>
#include <iostream>

//获取日志的接口
Logger& Logger::GetInstance()
{
  static Logger logger;
  return logger;
}

//私有构造
Logger::Logger()
{
  //启动一个独立的线程，负责从队列中取信息，写入日志文件
  std::thread writeLogTask([&]() {
    //独立线程 循环运行 不中断
    while (true) {
      //获取当前日期以命名文件，追加打开文件，获取当前时间，从队列取出日志信息，带上时间前缀写入文件，关闭文件
      time_t now = time(nullptr);
      struct tm* tm_now = localtime(&now);

      //以日期格式命名文件
      char filename[128]{};
      sprintf(filename, "%d-%d-%d-log.txt",
        tm_now->tm_year + 1900, tm_now->tm_mon + 1, tm_now->tm_mday);
      
      //以追加的方式打开文件
      FILE* pf = fopen(filename, "a+");
      if (pf == nullptr) {
        std::cout << "logger file: " << filename << " open error!" << std::endl;
        exit(EXIT_FAILURE);
      }

      //从队列中取出日志信息
      std::string message = m_lockQue.Pop();
      //准备时间前缀
      char time_prefix[32]{};
      sprintf(time_prefix, "[%s][%02d:%02d:%02d] ",
        (m_loglevel == static_cast<unsigned int>(LogLevel::INFO) ? "INFO" : "ERROR"),
        tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec);
      //时间前缀 + 日志信息 + 换行符，拼接成完整的日志信息
      message.insert(0, time_prefix);
      message.append("\n");

      //写入日志文件
      fputs(message.c_str(), pf);

      //关闭文件
      fclose(pf);
    }
  });
  //设置分离线程，守护线程
  writeLogTask.detach();
}

//设置日志级别
void Logger::SetLogLevel(const unsigned int loglevel)
{
  m_loglevel = loglevel;
}

//写日志 将日志信息写入lockqueue缓冲队列中
void Logger::Log(const std::string& message)
{
  m_lockQue.Push(message);
}

