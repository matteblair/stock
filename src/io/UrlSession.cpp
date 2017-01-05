//
// Created by Matt Blair on 1/1/17.
//

#include "io/UrlSession.hpp"
#include "io/Log.hpp"
#include <assert.h>
#include <curl/curl.h>
#include <poll.h>
#include <unistd.h>

namespace stock {

static uint32_t g_urlSessionEnvironments = 0;

UrlSession::Environment::Environment() {
  curl_global_init(CURL_GLOBAL_ALL);
  g_urlSessionEnvironments++;
}

UrlSession::Environment::~Environment() {
  curl_global_cleanup();
  g_urlSessionEnvironments--;
}

UrlSession::UrlSession(Options options) : m_options(options) {
  assert(g_urlSessionEnvironments > 0);

  // Create a pipe to send new and canceled requests to the curl thread.
  int addRequestFds[2];
  int cancelRequestFds[2];
  if (pipe(addRequestFds) == -1 || pipe(cancelRequestFds) == -1) {
    Log::e("UrlSession failed to create pipes!");
    return;
  }
  m_addRequestReadPipe = addRequestFds[0];
  m_addRequestWritePipe = addRequestFds[1];
  m_cancelRequestReadPipe = cancelRequestFds[0];
  m_cancelRequestWritePipe = cancelRequestFds[1];

  // Start curl thread.
  m_keepRunning = true;
  m_thread = std::move(std::thread(&UrlSession::curlLoop, this));
}

UrlSession::~UrlSession() {
  // Stop the curl thread.
  m_keepRunning = false;
  // Send a phony cancellation to the curl thread.
  RequestHandle handle = nullptr;
  write(m_cancelRequestWritePipe, &handle, sizeof(RequestHandle));
  m_thread.join();
}

UrlSession::RequestHandle UrlSession::addRequest(const std::string& url, OnCompleteCallback* callback, void* user) {
  // Set up an easy handle for this request.
  // TODO: Pool easy handles for reuse.
  auto handle = curl_easy_init();
  curl_easy_setopt(handle, CURLOPT_URL, url.c_str());
  curl_easy_setopt(handle, CURLOPT_HEADER, 0L);
  curl_easy_setopt(handle, CURLOPT_VERBOSE, 0L);
  curl_easy_setopt(handle, CURLOPT_ACCEPT_ENCODING, "gzip");
  curl_easy_setopt(handle, CURLOPT_CONNECTTIMEOUT_MS, m_options.connectionTimeoutMs);
  curl_easy_setopt(handle, CURLOPT_TIMEOUT_MS, m_options.requestTimeoutMs);

  // Configure the request object.
  Request request;
  request.handle = handle;
  request.callback = callback;
  request.user = user;

  // Send the request to the curl thread.
  Log::vf("UrlSession sending request to add with handle: %u\n", handle);
  write(m_addRequestWritePipe, &request, sizeof(Request));

  // Return the handle.
  return handle;
}

void UrlSession::cancelRequest(RequestHandle handle) {
  // Send canceled request to curl thread.
  Log::vf("UrlSession sending request to cancel with handle: %u\n", handle);
  write(m_cancelRequestWritePipe, &handle, sizeof(RequestHandle));
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

void UrlSession::curlLoop() {
  Log::v("curlLoop starting");

  // Initialize our multi handle.
  CURLM* multiHandle = curl_multi_init();
  curl_multi_setopt(multiHandle, CURLMOPT_MAXCONNECTS, m_options.maxConnections);

  // Create file descriptors to notify curl when requests have changed.
  curl_waitfd extraFds[2];
  extraFds[0] = { m_addRequestReadPipe, CURL_WAIT_POLLIN, 0 };
  extraFds[1] = { m_cancelRequestReadPipe, CURL_WAIT_POLLIN, 0 };
  pollfd addRequestPollFd = { m_addRequestReadPipe, POLLIN, 0 };
  pollfd cancelRequestPollFd = { m_cancelRequestReadPipe, POLLIN, 0 };

  // Create container for ongoing tasks.
  int runningHandles = 0;

  // Loop until the session is destroyed.
  while (m_keepRunning) {

    // Perform current tasks.
    auto code = curl_multi_perform(multiHandle, &runningHandles);
    Log::vf("curlLoop did curl_multi_perform, handles running: %d\n", runningHandles);
    if (code != CURLM_OK) {
      Log::ef("curlLoop curl_multi error code: %d\n", code);
      break;
    }

    // Wait until we are notified by an ongoing request or a request change.
    int notifiedHandles = -1;
    curl_multi_wait(multiHandle, extraFds, 2, 3000, &notifiedHandles);
    Log::vf("curlLoop did curl_multi_wait, handles notified: %d\n", notifiedHandles);

    // Get added requests.
    while (poll(&addRequestPollFd, 1, 0)) {
      Request request;
      read(m_addRequestReadPipe, &request, sizeof(Request));
      Log::vf("curlLoop adding request with handle: %u\n", request.handle);
      Task* task = new Task();
      task->request = request;
      curl_easy_setopt(request.handle, CURLOPT_WRITEFUNCTION, &curlWriteCallback);
      curl_easy_setopt(request.handle, CURLOPT_WRITEDATA, &(task->response));
      curl_multi_add_handle(multiHandle, request.handle);
      m_tasks.push_back(task);
    }

    // Get canceled requests.
    while (poll(&cancelRequestPollFd, 1, 0)) {
      RequestHandle handle;
      read(m_cancelRequestReadPipe, &handle, sizeof(RequestHandle));
      Log::vf("curlLoop canceling request with handle: %u\n", handle);
      if (finishRequest(handle, false, true)) {
        curl_multi_remove_handle(multiHandle, handle);
        curl_easy_cleanup(handle);
      }
    }

    // Check messages from curl multi.
    if (notifiedHandles > 0) {
      int queuedMessages = 0;
      while (auto* message = curl_multi_info_read(multiHandle, &queuedMessages)) {
        Log::v("Got curl_multi message");
        auto handle = message->easy_handle;
        if (message->msg == CURLMSG_DONE) {
          auto resultCode = message->data.result;
          if (finishRequest(handle, true, false)) {
            curl_multi_remove_handle(multiHandle, handle);
            curl_easy_cleanup(handle);
          }
        } else {
          Log::ef("curl_multi error msg: %d\n", message->msg);
        }
      }
    }
  }

  // Cancel all remaining tasks.
  for (auto task : m_tasks) {
    auto& request = task->request;
    auto& response = task->response;
    response.successful = false;
    response.canceled = true;
    if (request.callback) {
      request.callback(response, request.user);
    }
    curl_multi_remove_handle(multiHandle, request.handle);
    curl_easy_cleanup(request.handle);
    delete task;
  }
  m_tasks.clear();

  // Cleanup our multi handle.
  curl_multi_cleanup(multiHandle);
  Log::v("curlLoop exiting");
}

bool UrlSession::finishRequest(RequestHandle handle, bool successful, bool canceled) {
  for (auto it = m_tasks.begin(), end = m_tasks.end(); it != end; ++it) {
    auto& request = (*it)->request;
    auto& response = (*it)->response;
    if (request.handle == handle) {
      response.successful = successful;
      response.canceled = canceled;
      if (request.callback) {
        request.callback(response, request.user);
      }
      delete *it;
      m_tasks.erase(it);
      return true;
    }
  }
  return false;
}

} // namespace stock
