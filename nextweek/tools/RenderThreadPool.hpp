#ifndef __RENDER_THREAD_POOL__
#define __RENDER_THREAD_POOL__
#include <Windows.h>
#include <vector>
#include <queue>

struct RenderTaskParam { int from, to; };
class RenderThreadPool;
typedef void (*RenderTask)(RenderTaskParam);
DWORD WINAPI DispatchTask(LPVOID);

class RenderThreadPool {
    CRITICAL_SECTION cs;
    int threads_num;
    HANDLE* threads_handle;
    std::queue<std::pair<RenderTask, RenderTaskParam>> tasks;

    void CreateThreads();
public:
    RenderThreadPool(int num = 8) noexcept;
    ~RenderThreadPool() noexcept;
    void Lock();
    void UnLock();

    void AddTask(RenderTask task, RenderTaskParam param);
    std::pair<RenderTask, RenderTaskParam> GetTask();
    void WaitForTaskEnding();
    void Dispatch();
    int GetRestTasksNum();
};
#endif