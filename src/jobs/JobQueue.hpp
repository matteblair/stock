//
// Created by Matt Blair on 2019-03-02.
//

#pragma once

#include <functional>
#include <mutex>
#include <queue>

namespace stock {

using Job = std::function<void()>;

class JobQueue {

public:

  JobQueue() = default;

  void enqueue(Job&& job);

  bool dequeue(Job& job);

private:

  std::queue<Job> m_jobs;

  std::mutex m_mutex;

};

} // namespace stock
