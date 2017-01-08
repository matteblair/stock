//
// Created by Matt Blair on 1/1/17.
//

#include "io/UrlSession.hpp"
#include "io/Log.hpp"
#include <cassert>
#include <cstring>
#include <curl/curl.h>

namespace stock {

static uint32_t globalUrlSessionEnvironments = 0;

UrlSession::Environment::Environment() {
  curl_global_init(CURL_GLOBAL_ALL);
  globalUrlSessionEnvironments++;
}

UrlSession::Environment::~Environment() {
  curl_global_cleanup();
  globalUrlSessionEnvironments--;
}

UrlSession::Response getCanceledResponse() {
  UrlSession::Response response;
  response.canceled = true;
  return response;
}

UrlSession::UrlSession(Options options) : m_options(options) {
  assert(globalUrlSessionEnvironments > 0);
  assert(options.numberOfThreads > 0);
  // Start the curl threads.
  m_keepRunning = true;
  m_tasks.resize(options.numberOfThreads);
  for (uint32_t i = 0; i < options.numberOfThreads; i++) {
    m_threads.emplace_back(&UrlSession::curlLoop, this, i);
  }
}

UrlSession::~UrlSession() {
  // Make all tasks cancelled.
  {
    std::lock_guard<std::mutex> lock(m_requestMutex);
    for (auto& request : m_requests) {
      if (request.callback) {
        auto response = getCanceledResponse();
        request.callback(response);
      }
    }
    m_requests.clear();
    for (auto& task : m_tasks) {
      task.response.canceled = true;
    }
  }
  // Stop the curl threads.
  m_keepRunning = false;
  m_requestCondition.notify_all();
  for (auto& thread : m_threads) {
    thread.join();
  }
}

UrlSession::RequestHandle UrlSession::addRequest(std::string url, CompletionCallback onComplete) {
  // Create a new request.
  Request request = { url, onComplete, ++m_requestIndex };
  // Add the request to our list.
  {
    // Lock the mutex to prevent concurrent modification of the list by the curl loop thread.
    std::lock_guard<std::mutex> lock(m_requestMutex);
    m_requests.push_back(request);
  }
  // Notify a thread to start the transfer.
  m_requestCondition.notify_one();
  // Return the handle.
  return m_requestIndex;
}

void UrlSession::cancelRequest(RequestHandle handle) {
  std::lock_guard<std::mutex> lock(m_requestMutex);
  // First check the pending request list.
  for (auto it = m_requests.begin(), end = m_requests.end(); it != end; ++it) {
    auto& request = *it;
    if (request.handle == handle) {
      // Found the request! Now run its callback and remove it.
      auto response = getCanceledResponse();
      if (request.callback) {
        request.callback(response);
      }
      m_requests.erase(it);
      return;
    }
  }
  // Next check the active request list.
  for (auto& task : m_tasks) {
    if (task.request.handle == handle) {
      task.response.canceled = true;
    }
  }
}

size_t curlWriteCallback(char* ptr, size_t size, size_t n, void* user) {
  // Writes data received by libCURL.
  auto* response = reinterpret_cast<UrlSession::Response*>(user);
  auto& buffer = response->data;
  auto addedSize = size * n;
  auto oldSize = buffer.size();
  buffer.resize(oldSize + addedSize);
  std::memcpy(buffer.data() + oldSize, ptr, addedSize);
  return addedSize;
}

int curlProgressCallback(void* user, double dltotal, double dlnow, double ultotal, double ulnow) {
  // Signals libCURL to abort the request if marked as canceled.
  auto* response = reinterpret_cast<UrlSession::Response*>(user);
  return static_cast<int>(response->canceled);
}

void UrlSession::curlLoop(uint32_t index) {
  assert(m_tasks.size() > index);
  Task& task = m_tasks[index];
  Log::vf("curlLoop %u starting\n", index);
  // Create a buffer for curl error messages.
  char curlErrorString[CURL_ERROR_SIZE];
  // Set up an easy handle for reuse.
  auto handle = curl_easy_init();
  curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, &curlWriteCallback);
  curl_easy_setopt(handle, CURLOPT_WRITEDATA, &task.response);
  curl_easy_setopt(handle, CURLOPT_PROGRESSFUNCTION, &curlProgressCallback);
  curl_easy_setopt(handle, CURLOPT_PROGRESSDATA, &task.response);
  curl_easy_setopt(handle, CURLOPT_NOPROGRESS, 0L);
  curl_easy_setopt(handle, CURLOPT_HEADER, 0L);
  curl_easy_setopt(handle, CURLOPT_VERBOSE, 0L);
  curl_easy_setopt(handle, CURLOPT_ACCEPT_ENCODING, "gzip");
  curl_easy_setopt(handle, CURLOPT_ERRORBUFFER, curlErrorString);
  curl_easy_setopt(handle, CURLOPT_CONNECTTIMEOUT_MS, m_options.connectionTimeoutMs);
  curl_easy_setopt(handle, CURLOPT_TIMEOUT_MS, m_options.requestTimeoutMs);
  // Loop until the session is destroyed.
  while (m_keepRunning) {
    bool haveRequest = false;
    // Wait until the condition variable is notified.
    {
      std::unique_lock<std::mutex> lock(m_requestMutex);
      if (m_requests.empty()) {
        Log::vf("curlLoop %u waiting\n", index);
        m_requestCondition.wait(lock);
      }
      Log::vf("curlLoop %u notified\n", index);
      // Try to get a request from the list.
      if (!m_requests.empty()) {
        // Take the first request from our list.
        task.request = m_requests.front();
        m_requests.erase(m_requests.begin());
        haveRequest = true;
      }
    }
    if (haveRequest) {
      // Configure the easy handle.
      const char* url = task.request.url.data();
      curl_easy_setopt(handle, CURLOPT_URL, url);
      Log::vf("curlLoop %u starting request for url: %s\n", index, url);
      // Perform the request.
      auto result = curl_easy_perform(handle);
      // Get the result status code.
      long httpStatus = 0;
      curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &httpStatus);
      // Handle success or error.
      if (result == CURLE_OK && httpStatus >= 200 && httpStatus < 300) {
        Log::vf("curlLoop %u succeeded with http status: %d for url: %s\n", index, httpStatus, url);
        task.response.successful = true;
      } else if (result == CURLE_ABORTED_BY_CALLBACK) {
        Log::vf("curlLoop %u request aborted for url: %s\n", index, url);
        task.response.successful = false;
      } else {
        Log::ef("curlLoop %u failed: '%s' with http status: %d for url: %s\n", index, curlErrorString, httpStatus, url);
        task.response.successful = false;
      }
      if (task.request.callback) {
        Log::vf("curlLoop %u performing request callback\n", index);
        task.request.callback(task.response);
      }
    }
    // Reset the response.
    task.response.data.clear();
    task.response.canceled = false;
    task.response.successful = false;
  }
  Log::vf("curlLoop %u exiting\n", index);
  // Clean up our easy handle.
  curl_easy_cleanup(handle);
}

} // namespace stock
