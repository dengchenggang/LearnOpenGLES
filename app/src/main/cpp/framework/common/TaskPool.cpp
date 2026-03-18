#include "TaskPool.h"
#include "LogUtils.h"

KEY_VALUE(TAG, TaskPool)

TaskPool::TaskPool() = default;

TaskPool::~TaskPool() {
    stop();
}

void TaskPool::start() {
    if (mRunning) {
        return;
    }

    mRunning = true;
    mStopRequested = false;
    mTasksTotal = 0;

    // 启动工作线程
    mWorker = std::thread(&TaskPool::workerLoop, this);

    LogI("%s started", TAG);
}

void TaskPool::stop() {
    if (!mRunning) {
        return;
    }

    mStopRequested = true;

    // 唤醒工作线程（包括正在执行延时任务的线程）
    mCondition.notify_one();

    // 等待工作线程结束
    if (mWorker.joinable()) {
        mWorker.join();
    }

    mRunning = false;

    // 清空任务队列
    {
        std::lock_guard<std::mutex> lock(mMutex);
        while (!mTasks.empty()) {
            mTasks.pop();
        }
    }

    mTasksTotal = 0;
    LogI("%s stopped", TAG);
}

void TaskPool::workerLoop() {
    while (!mStopRequested) {
        std::function<void()> taskFunc;
        bool hasTask = false;

        {
            std::unique_lock<std::mutex> lock(mMutex);

            // 如果没有任务，一直等待直到有新任务或停止请求
            if (mTasks.empty()) {
                mCondition.wait(lock, [this] {
                    return !mTasks.empty() || mStopRequested;
                });
            }

            // 如果队列不为空但任务未到期，计算等待时间
            if (!mTasks.empty() && !mTasks.top().isReady()) {
                auto now = std::chrono::steady_clock::now();
                auto delay = mTasks.top().executeTime - now;

                // 等待直到任务到期或有新任务（新任务可能更早到期或更高优先级）
                // LogI("TaskPool workerLoop: delay=%lld", delay.count());
                mCondition.wait_for(lock, delay, [this] {
                    return mStopRequested ||
                           (mTasks.top().isReady()) ||  // 任务到期
                           false;  // 也可以检查是否有更高优先级的任务加入
                });
            }

            if (mStopRequested) {
                break;
            }
            bool isTopReady = mTasks.top().isReady();
            // 检查是否有到期的任务
            if (!mTasks.empty() && isTopReady) {
                // priority_queue::top() 返回 const 引用，需要 const_cast 来移动
                taskFunc = std::move(const_cast<Task&>(mTasks.top()).func);
                mTasks.pop();
                --mTasksTotal;
                hasTask = true;
            }
        }

        if (hasTask && taskFunc) {
            try {
                taskFunc();
            } catch (...) {
                // 捕获异常，防止线程崩溃
            }
        }
    }
}
