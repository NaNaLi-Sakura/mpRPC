/*
 程序 : lockqueue.hpp

 功能 : 线程安全的缓冲队列

 作者 : 雪与冰心丶
*/

#ifndef LOCKQUEUE_H
#define LOCKQUEUE_H

#include <queue>
#include <thread>
#include <mutex> //pthread_mutex_t
#include <condition_variable> //pthread_condition_t

/// @brief 异步日志系统的日志队列
/// @tparam T 队列数据类型
template<typename T>
class LockQueue
{
private:
  std::queue<T> m_queue; //队列
  std::mutex m_mutex; //队列的互斥锁
  std::condition_variable m_condvariable; //队列的条件变量  
public:
  //入队
  void Push(const T& data);
  //出队
  T Pop();
};

//入队
template<typename T>
void LockQueue<T>::Push(const T& data)
{
  //加锁 入队 通知消费者消费 解锁
  std::lock_guard<std::mutex> lg(m_mutex);
  m_queue.push(data);
  m_condvariable.notify_one(); //负责消费日志的线程仅有一个独立线程
}
//出队
template<typename T>
T LockQueue<T>::Pop()
{
  //加锁 当队列为空时进入wait状态等待信号并释放锁，被唤醒之后进入阻塞状态等待获取锁，
  //在生产者线程将锁释放掉之后，此消费者线程抢到锁(加锁)，再次回到while循环，
  //若此时队列不为空，则开始处理数据
  std::unique_lock<std::mutex> ulk(m_mutex);
  while (m_queue.empty()) {
    //日志队列为空时，释放锁，进入wait状态，等待生产者的唤醒信号
    m_condvariable.wait(ulk);
  }

  //队列不为空，出队
  T data = m_queue.front(); m_queue.pop();
  
  return data;
}


#endif //LOCKQUEUE_H