//
// Created by Matt Blair on 2019-03-04.
//

#pragma once

#include "jobs/JobQueue.hpp"
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>

namespace stock {

class WorkerPool {

public:

  explicit WorkerPool(size_t numberOfWorkers);

  ~WorkerPool();

  void dispose();

  void enqueue(Job&& job);

protected:

  void workerLoop();

  JobQueue m_jobQueue;
  std::vector<std::thread> m_threads;
  std::condition_variable m_condition;
  std::mutex m_mutex;
  std::atomic<bool> m_disposed;

};

} // namespace stock
