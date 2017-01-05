//
// Created by Matt Blair on 1/1/17.
//

#pragma once
#include <string>
#include <thread>
#include <vector>

namespace stock {

class UrlSession {

public:

  struct Environment {
    Environment();
    ~Environment();
  };

  struct Options {
    uint32_t maxConnections = 16;
    uint32_t connectionTimeoutMs = 3000;
    uint32_t requestTimeoutMs = 30000;
  };

  UrlSession(Options options);
  ~UrlSession();

  struct Response {
    std::vector<uint8_t> data;
    bool successful = false;
    bool canceled = false;
  };

  using OnCompleteCallback = void(const Response& response, void* user);

  using RequestHandle = void*;

  RequestHandle addRequest(const std::string& url, OnCompleteCallback* callback, void* user);

  void cancelRequest(RequestHandle request);

private:

  struct Request {
    RequestHandle handle = 0;
    OnCompleteCallback* callback = nullptr;
    void* user = nullptr;
  };

  struct Task {
    Request request;
    Response response;
  };

  void curlLoop();

  bool finishRequest(RequestHandle handle, bool successful, bool canceled);

  std::thread m_thread;
  std::vector<Task*> m_tasks;
  Options m_options;
  int m_addRequestWritePipe = -1;
  int m_addRequestReadPipe = -1;
  int m_cancelRequestWritePipe = -1;
  int m_cancelRequestReadPipe = -1;
  bool m_keepRunning = false;

};

} // namespace stock
