//
// Created by Matt Blair on 1/1/17.
//

#include "io/UrlSession.hpp"
#include "io/Log.hpp"
#include <cassert>
#include <cstring>
#include <curl/curl.h>

namespace stock {

void UrlSession::globalInit() {
  curl_global_init(CURL_GLOBAL_ALL);
}

UrlSession::UrlSession(Options options) : m_options(options) {
  assert(options.numberOfThreads > 0);
  // Start the curl threads.
  m_keepRunning = true;
  for (uint32_t i = 0; i < options.numberOfThreads; i++) {
    m_threads.emplace_back(&UrlSession::curlLoop, this);
  }
}

UrlSession::~UrlSession() {
  // Make all tasks cancelled.
  {
    Response cancelledResponse;
    cancelledResponse.canceled = true;
    std::lock_guard<std::mutex> lock(m_requestMutex);
    for (auto& request : m_pendingRequests) {
      if (request.callback) {
        request.callback(cancelledResponse);
      }
    }
    m_pendingRequests.clear();
    for (auto& request : m_activeRequests) {
      if (request.response) {
        request.response->canceled = true;
      }
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
  Request request;
  request.url = url;
  request.callback = onComplete;
  request.handle = ++m_requestIndex;
  // Add the request to our list.
  {
    // Lock the mutex to prevent concurrent modification of the list by the curl loop thread.
    std::lock_guard<std::mutex> lock(m_requestMutex);
    m_pendingRequests.push_back(request);
  }
  // Notify a thread to start the transfer.
  m_requestCondition.notify_one();
  // Return the handle.
  return m_requestIndex;
}

void UrlSession::cancelRequest(RequestHandle handle) {
  std::lock_guard<std::mutex> lock(m_requestMutex);
  // First check the pending request list.
  for (auto it = m_pendingRequests.begin(), end = m_pendingRequests.end(); it != end; ++it) {
    auto& request = *it;
    if (request.handle == handle) {
      // Found the request! Now run its callback and remove it.
      Response response;
      response.canceled = true;
      if (request.callback) {
        request.callback(response);
      }
      m_pendingRequests.erase(it);
      return;
    }
  }
  // Next check the active request list.
  for (auto it = m_activeRequests.begin(), end = m_activeRequests.end(); it != end; ++it) {
    auto& request = *it;
    if (request.response) {
      request.response->canceled = true;
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

void UrlSession::curlLoop() {
  auto id = std::hash<std::thread::id>()(std::this_thread::get_id());
  Log::vf("curlLoop starting on thread: %u\n", id);
  // Set up a response for reuse.
  Response response;
  // Set up an easy handle for reuse.
  auto handle = curl_easy_init();
  curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, &curlWriteCallback);
  curl_easy_setopt(handle, CURLOPT_WRITEDATA, &response);
  curl_easy_setopt(handle, CURLOPT_PROGRESSFUNCTION, &curlProgressCallback);
  curl_easy_setopt(handle, CURLOPT_PROGRESSDATA, &response);
  curl_easy_setopt(handle, CURLOPT_NOPROGRESS, 0L);
  curl_easy_setopt(handle, CURLOPT_HEADER, 0L);
  curl_easy_setopt(handle, CURLOPT_VERBOSE, 0L);
  curl_easy_setopt(handle, CURLOPT_ACCEPT_ENCODING, "gzip");
  curl_easy_setopt(handle, CURLOPT_CONNECTTIMEOUT_MS, m_options.connectionTimeoutMs);
  curl_easy_setopt(handle, CURLOPT_TIMEOUT_MS, m_options.requestTimeoutMs);
  // Loop until the session is destroyed.
  while (m_keepRunning) {
    Request* request = nullptr;
    // Wait until the condition variable is notified.
    {
      std::unique_lock<std::mutex> lock(m_requestMutex);
      if (m_pendingRequests.empty()) {
        Log::vf("curlLoop waiting on thread: %u\n", id);
        m_requestCondition.wait(lock);
      }
      Log::vf("curlLoop notified on thread: %u\n", id);
      // Try to get a request from the list.
      if (!m_pendingRequests.empty()) {
        // Transfer the first request from the pending list to the end of the active list.
        m_activeRequests.splice(m_activeRequests.end(), m_pendingRequests, m_pendingRequests.begin());
        request = &m_activeRequests.back();
        request->response = &response;
      }
    }
    if (request) {
      Log::vf("curlLoop starting request on thread: %u\n", id);
      // Got a request!
      // Configure the easy handle.
      const char* url = request->url.data();
      curl_easy_setopt(handle, CURLOPT_URL, url);
      // Perform the request.
      auto result = curl_easy_perform(handle);
      // Get the result status code.
      long httpStatus = 0;
      curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &httpStatus);
      // Handle success or error.
      if (result == CURLE_OK && httpStatus >= 200 && httpStatus < 300) {
        Log::vf("curlLoop easy_perform succeeded with http status: %d for url: %s\n", httpStatus, url);
        response.successful = true;
      } else if (result == CURLE_ABORTED_BY_CALLBACK) {
        Log::vf("curlLoop easy_perform aborted by callback of url: %s\n", url);
        response.successful = false;
      } else {
        Log::ef("curlLoop easy_perform failed with code: %d and http status: %d for url: %s\n", result, httpStatus, url);
        response.successful = false;
      }
      Log::vf("curlLoop completed request on thread: %u\n", id);
      if (request->callback) {
        request->callback(response);
      }
    }
    // Reset the response.
    response.data.clear();
    response.canceled = false;
    response.successful = false;
  }
  Log::vf("curlLoop exiting on thread: %u\n", id);
  // Clean up our easy handle.
  curl_easy_cleanup(handle);
}

} // namespace stock
