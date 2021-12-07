#include "RenderThreadPool.hpp"

DWORD WINAPI DispatchTask(LPVOID pool_) {
    RenderThreadPool* pool = (RenderThreadPool*) pool_;
    while (true) {
        pool->Lock();
        if (pool->GetRestTasksNum() <= 0) {
            pool->UnLock();
            break;
        }
        
        auto [task, param] = pool->GetTask();
        pool->UnLock();
        (*task)(param);
    }
    return 0L;
}

void RenderThreadPool::CreateThreads() {
    for (int i = 0; i < threads_num; i++) {
        threads_handle[i] = CreateThread(NULL, 0, DispatchTask, this, 0, NULL);
    }
}

RenderThreadPool::RenderThreadPool(int num) noexcept {
    InitializeCriticalSection(&cs);
    threads_num = num;
    threads_handle = new HANDLE[num];
}

RenderThreadPool::~RenderThreadPool() noexcept {
    DeleteCriticalSection(&cs);
    if (threads_handle != nullptr) delete[] threads_handle;
}

void RenderThreadPool::Lock() { EnterCriticalSection(&cs); }
void RenderThreadPool::UnLock() { LeaveCriticalSection(&cs); }

void RenderThreadPool::AddTask(RenderTask task, RenderTaskParam param) {
    tasks.emplace(std::make_pair(task, param));
}
std::pair<RenderTask, RenderTaskParam> RenderThreadPool::GetTask() {
    auto ret = tasks.front();
    tasks.pop();
    return ret;
}

void RenderThreadPool::WaitForTaskEnding() {
    WaitForMultipleObjects(threads_num, threads_handle, TRUE, INFINITE);
}

void RenderThreadPool::Dispatch() {
    CreateThreads();
}

int RenderThreadPool::GetRestTasksNum() {
    return tasks.size();
}