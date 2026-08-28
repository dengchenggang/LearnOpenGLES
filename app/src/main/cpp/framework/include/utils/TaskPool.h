#ifndef TASK_POOL_H
#define TASK_POOL_H

#include <functional>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <atomic>
#include <chrono>
#include <vector>
#include <memory>
#include <future>
#include <type_traits>
#include <utility>

// 单线程任务池（专门用于渲染线程）
class TaskPool {
private:
    // 任务结构，支持延时执行
    struct Task {
        std::function<void()> func;
        std::chrono::steady_clock::time_point executeTime;  // 计划执行时间

        Task() : executeTime(std::chrono::steady_clock::now()) {}

        // 立即执行任务
        explicit Task(std::function<void()> f)
            : func(std::move(f)), executeTime(std::chrono::steady_clock::now()) {}

        // 延时执行任务
        Task(std::function<void()> f, std::chrono::steady_clock::time_point execTime)
            : func(std::move(f)), executeTime(execTime) {}

        // 支持移动
        Task(Task&&) = default;
        Task& operator=(Task&&) = default;

        // 禁止拷贝
        Task(const Task&) = delete;
        Task& operator=(const Task&) = delete;

        // 检查任务是否到期
        bool isReady() const {
            return std::chrono::steady_clock::now() >= executeTime;
        }

        // 按执行时间排序（时间早的优先）
        bool operator<(const Task& other) const {
            return executeTime > other.executeTime;  // priority_queue 默认是大顶堆，所以用 >
        }
    };

public:
    TaskPool();
    ~TaskPool();

    // 禁止拷贝和移动
    TaskPool(const TaskPool&) = delete;
    TaskPool& operator=(const TaskPool&) = delete;
    TaskPool(TaskPool&&) = delete;
    TaskPool& operator=(TaskPool&&) = delete;

    // 启动线程
    void start();

    // 停止线程
    void stop();

    // 提交任务并返回 future（可以获取返回值）
    // 参考 BS_thread_pool::submit_task
    template<typename F, typename... Args>
    auto submit(F&& func, Args&&... args) -> std::future<std::invoke_result_t<F, Args...>> {
        using ReturnType = std::invoke_result_t<F, Args...>;

        auto packagedTask = std::make_shared<std::packaged_task<ReturnType()>>(
            std::bind(std::forward<F>(func), std::forward<Args>(args)...)
        );

        std::future<ReturnType> result = packagedTask->get_future();

        {
            std::lock_guard<std::mutex> lock(mMutex);
            mTasks.emplace([packagedTask]() { (*packagedTask)(); });
        }
        mCondition.notify_one();
        ++mTasksTotal;

        return result;
    }

    // 提交任务不等待结果（fire and forget）
    // 参考 BS_thread_pool::detach_task
    template<typename F, typename... Args>
    void detach(F&& func, Args&&... args) {
        {
            std::lock_guard<std::mutex> lock(mMutex);
            mTasks.emplace(std::bind(std::forward<F>(func), std::forward<Args>(args)...));
        }
        mCondition.notify_one();
        ++mTasksTotal;
    }

    // 提交延时任务（毫秒），返回 future
    template<typename F, typename... Args>
    auto submitDelayed(int64_t delayMs, F&& func, Args&&... args) -> std::future<std::invoke_result_t<F, Args...>> {
        using ReturnType = std::invoke_result_t<F, Args...>;

        auto packagedTask = std::make_shared<std::packaged_task<ReturnType()>>(
            std::bind(std::forward<F>(func), std::forward<Args>(args)...)
        );

        std::future<ReturnType> result = packagedTask->get_future();
        auto execTime = std::chrono::steady_clock::now() + std::chrono::milliseconds(delayMs);

        {
            std::lock_guard<std::mutex> lock(mMutex);
            mTasks.emplace([packagedTask]() { (*packagedTask)(); }, execTime);
        }
        mCondition.notify_one();
        ++mTasksTotal;

        return result;
    }

    // 提交延时任务不等待结果
    template<typename F, typename... Args>
    void detachDelayed(int64_t delayMs, F&& func, Args&&... args) {
        auto execTime = std::chrono::steady_clock::now() + std::chrono::milliseconds(delayMs);

        {
            std::lock_guard<std::mutex> lock(mMutex);
            mTasks.emplace(std::bind(std::forward<F>(func), std::forward<Args>(args)...), execTime);
        }
        mCondition.notify_one();
        ++mTasksTotal;
    }

    // 获取待处理任务数量
    size_t getTasksTotal() const { return mTasksTotal; }

    // 检查是否运行中
    bool isRunning() const { return mRunning; }

private:
    void workerLoop();

private:
    std::thread mWorker;

    // 优先级任务队列（优先级高的先出队）
    std::priority_queue<Task> mTasks;

    mutable std::mutex mMutex;
    std::condition_variable mCondition;

    std::atomic<bool> mRunning{false};
    std::atomic<bool> mStopRequested{false};
    std::atomic<size_t> mTasksTotal{0};
};

#endif // TASK_POOL_H
