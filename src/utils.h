// utils.h

#ifndef UTILS_H
#define UTILS_H

#include <condition_variable>
#include <mutex>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <ctime>

class CountdownLatch
{
private:
  std::mutex mtx;
  std::condition_variable cond;
  int v;

public:
  CountdownLatch();
  CountdownLatch(int v);

  void await();
  void countDown();
  void countUp();
};

long long getCurTime();

std::string getRandomID();

#endif // UTILS_H
