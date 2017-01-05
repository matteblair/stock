//
// Created by Matt Blair on 1/1/17.
//

#pragma once
#include <condition_variable>
#include <functional>
#include <list>
#include <mutex>
#include <thread>
#include <string>
#include <vector>

namespace stock {

class UrlSession {

public:

  struct Options {
    uint32_t numberOfThreads = 4;
    uint32_t connectionTimeoutMs = 3000;
    uint32_t requestTimeoutMs = 30000;
  };

  struct Response {
    std::vector<uint8_t> data;
    bool successful = false;
    bool canceled = false;
  };

  using CompletionCallback = std::function<void(Response& response)>;

  using RequestHandle = uint32_t;

  // Must be called at least once before any session is created or used.
  static void globalInit();

  UrlSession(Options options);
  ~UrlSession();

  RequestHandle addRequest(std::string url, CompletionCallback onComplete);

  void cancelRequest(RequestHandle request);

private:

  struct Request {
    std::string url;
    CompletionCallback callback;
    RequestHandle handle;
    Response* response = nullptr;
  };

  void curlLoop();

  std::vector<std::thread> m_threads;
  std::list<Request> m_pendingRequests;
  std::list<Request> m_activeRequests;
  std::condition_variable m_requestCondition;
  std::mutex m_requestMutex;
  Options m_options;
  RequestHandle m_requestIndex = 0;
  bool m_keepRunning = false;

};

} // namespace stock
