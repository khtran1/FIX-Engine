// CountdownLatch.h

#include <condition_variable>
#include <mutex>

class CountdownLatch
{
private:
  std::mutex mtx;
  std::condition_variable cond;
  int v;

public:
  CountdownLatch() : v(0) {}
  CountdownLatch(int v) : v(v) {}

  void await()
  {
    std::unique_lock<std::mutex> lock(mtx);
    cond.wait(lock, [this]
              { return v == 0; });
  }

  void countDown()
  {
    std::unique_lock<std::mutex> lock(mtx);
    if (v > 0)
    {
      if (--v == 0)
        cond.notify_all();
    }
  }

  void countUp() { ++v; }
};