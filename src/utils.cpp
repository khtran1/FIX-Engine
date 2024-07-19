// utils.cpp

#include <random>
#include "utils.h"

CountdownLatch::CountdownLatch() : v(0) {}

CountdownLatch::CountdownLatch(int v) : v(v) {}

void CountdownLatch::await()
{
  std::unique_lock<std::mutex> lock(mtx);
  cond.wait(lock, [this]
            { return v == 0; });
}

void CountdownLatch::countDown()
{
  std::unique_lock<std::mutex> lock(mtx);
  if (v > 0)
  {
    if (--v == 0)
      cond.notify_all();
  }
}

void CountdownLatch::countUp()
{
  ++v;
}

long long getCurTime()
{
  return std::chrono::duration_cast<std::chrono::milliseconds>(
             std::chrono::system_clock::now().time_since_epoch())
      .count();
}

std::string getRandomID()
{
  auto gen = std::mt19937(std::random_device{}());
  auto dist = std::uniform_int_distribution<int>{100000000, 999999999};

  return std::to_string(dist(gen));
}