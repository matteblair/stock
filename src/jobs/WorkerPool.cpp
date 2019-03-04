//
// Created by Matt Blair on 2019-03-04.
//

#include "WorkerPool.hpp"
#include <cassert>

namespace stock {

WorkerPool::WorkerPool(size_t numberOfWorkers) {
  m_disposed = false;
  for (size_t i = 0; i < numberOfWorkers; i++) {
    m_threads.emplace_back(&WorkerPool::workerLoop, this);
  }
}

void WorkerPool::dispose() {
  m_disposed = true;
  m_condition.notify_all();
  for (auto& thread : m_threads) {
    thread.join();
  }
}

WorkerPool::~WorkerPool() {
  assert(m_disposed);
}

void WorkerPool::enqueue(Job&& job) {
  m_jobQueue.enqueue(std::move(job));
  m_condition.notify_one();
}

void WorkerPool::workerLoop() {
  while (!m_disposed) {
    Job currentJob;
    if (m_jobQueue.dequeue(currentJob)) {
      currentJob();
    } else {
      // Wait until notified.
      std::unique_lock<std::mutex> lock(m_mutex);
      m_condition.wait(lock);
    }
  }
}

} // namespace stock
