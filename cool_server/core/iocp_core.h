#pragma once

#include <winsock2.h>
#include <windows.h>
#include <vector>
#include <thread>
#include <functional>
#include <atomic>

class IOCPCore {
public:
    using CompletionHandler = std::function<void(DWORD, ULONG_PTR, LPOVERLAPPED)>;

    IOCPCore();
    ~IOCPCore();

    bool setup();
    void associateSocket(SOCKET socket, ULONG_PTR key);
    void runWorkerThreads(int count, CompletionHandler handler);
    void stop();
    void postCompletion(DWORD bytesTransferred, ULONG_PTR key, LPOVERLAPPED overlapped);

private:
    HANDLE iocpHandle_;
    std::vector<std::thread> workerThreads_;
    std::atomic<bool> isRunning_;
    CompletionHandler handler_;
};