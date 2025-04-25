#include "iocp_core.h"
#include <iostream>

IOCPCore::IOCPCore() 
    : iocpHandle_(INVALID_HANDLE_VALUE), isRunning_(false) {}

IOCPCore::~IOCPCore() {
    stop();
}

bool IOCPCore::setup() {
    iocpHandle_ = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
    if (iocpHandle_ == NULL) {
        std::cerr << "CreateIoCompletionPort failed: " << GetLastError() << "\n";
        return false;
    }
    return true;
}

void IOCPCore::associateSocket(SOCKET socket, ULONG_PTR key) {
    if (CreateIoCompletionPort((HANDLE)socket, iocpHandle_, key, 0) == NULL) {
        std::cerr << "AssociateSocket failed: " << GetLastError() << "\n";
    }
}

void IOCPCore::runWorkerThreads(int count, CompletionHandler handler) {
    isRunning_ = true;
    handler_ = handler;
    
    for (int i = 0; i < count; ++i) {
        workerThreads_.emplace_back([this]() {
            while (isRunning_) {
                DWORD bytesTransferred = 0;
                ULONG_PTR completionKey = 0;
                LPOVERLAPPED overlapped = nullptr;

                BOOL success = GetQueuedCompletionStatus(
                    iocpHandle_,
                    &bytesTransferred,
                    &completionKey,
                    &overlapped,
                    INFINITE);

                if (!isRunning_) break;
                if (!overlapped) continue;

                handler_(bytesTransferred, completionKey, overlapped);
            }
        });
    }
}

void IOCPCore::stop() {
    if (!isRunning_) return;
    
    isRunning_ = false;
    for (size_t i = 0; i < workerThreads_.size(); ++i) {
        PostQueuedCompletionStatus(iocpHandle_, 0, 0, nullptr);
    }

    for (auto& thread : workerThreads_) {
        if (thread.joinable()) thread.join();
    }
    workerThreads_.clear();

    if (iocpHandle_ != INVALID_HANDLE_VALUE) {
        CloseHandle(iocpHandle_);
        iocpHandle_ = INVALID_HANDLE_VALUE;
    }
}

void IOCPCore::postCompletion(DWORD bytesTransferred, ULONG_PTR key, LPOVERLAPPED overlapped) {
    PostQueuedCompletionStatus(iocpHandle_, bytesTransferred, key, overlapped);
}
