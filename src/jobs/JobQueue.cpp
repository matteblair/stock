//
// Created by Matt Blair on 2019-03-02.
//

#include "JobQueue.hpp"

namespace stock {

void JobQueue::enqueue(Job&& job) {
  std::lock_guard<std::mutex> lock(m_mutex);
  m_jobs.push(std::move(job));
}

bool JobQueue::dequeue(Job& job) {
  std::lock_guard<std::mutex> lock(m_mutex);
  if (m_jobs.empty()) {
    return false;
  }
  job = std::move(m_jobs.front());
  m_jobs.pop();
  return true;
}

} // namespace stock
