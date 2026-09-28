#pragma once
#include <algorithm>
#include <atomic>
#include <exception>
#include <mutex>
#include <thread>
#include <vector>

namespace CONFIND::detail {
inline std::size_t WorkerCount(std::size_t tasks, std::size_t requested) {
  return tasks ? std::min(tasks, requested ? requested : std::size_t{1}) : 0;
}

// Operation-scoped bounded pool. Scheduling order never specifies result order.
template<class Task>
void RunIndexed(std::size_t tasks, std::size_t requested, Task&& task) {
  const auto count = WorkerCount(tasks, requested);
  if (count <= 1) {
    for (std::size_t k=0; k<tasks; ++k) task(0,k);
    return;
  }
  std::atomic<std::size_t> next{0};
  std::atomic<bool> cancelled{false};
  std::mutex error_mutex;
  std::exception_ptr first_error;
  std::vector<std::thread> workers;
  workers.reserve(count);
  auto join = [&] { for (auto& worker : workers) if (worker.joinable()) worker.join(); };
  auto run = [&](std::size_t w) {
    try {
      while (!cancelled.load(std::memory_order_relaxed)) {
        auto k = next.fetch_add(1, std::memory_order_relaxed);
        if (k >= tasks) break;
        task(w,k);
      }
    } catch (...) {
      { std::lock_guard<std::mutex> lock(error_mutex);
        if (!first_error) first_error = std::current_exception(); }
      cancelled.store(true, std::memory_order_relaxed);
    }
  };
  try { for (std::size_t w=0; w<count; ++w) workers.emplace_back(run,w); }
  catch (...) { cancelled.store(true, std::memory_order_relaxed); join(); throw; }
  join();
  if (first_error) std::rethrow_exception(first_error);
}
}
