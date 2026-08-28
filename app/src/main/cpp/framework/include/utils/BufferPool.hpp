# pragma once
#include <mutex>
#include <utility>
#include <memory>
#include <queue>
#include <map>

template <typename T>
class BufferPool {
public:
    /// @brief 构造缓冲池
    /// @param capacity 总容量上限（字节数）
    explicit BufferPool(size_t capacity) : mCapacity(capacity) {}

    ~BufferPool() = default;
    BufferPool(const BufferPool&) = delete;
    BufferPool& operator=(const BufferPool&) = delete;
public:
    /// @brief 从池中获取 buffer
    /// @param size buffer 规格大小（字节数）
    /// @param args 构造参数（池中无可用时动态创建使用）
    /// @return first: buffer 的 shared_ptr; second: true 表示从池中复用，false 表示新创建
    template <typename... Args>
    std::pair<std::shared_ptr<T>, bool> acquire(size_t size, Args&&... args);

private:
    /// @brief 将 buffer 归还池中
    /// @param size buffer 规格大小
    /// @param buffer 要归还的 buffer
    void release(size_t size, std::unique_ptr<T>&& buffer);
private:
    /// @brief 自定义删除器，shared_ptr 析构时自动归还 buffer 到池中
    template <typename U>
    struct BufferPoolDeleter {
        BufferPool<U>& pool;
        std::unique_ptr<U> buffer;
        const size_t size;

        BufferPoolDeleter(BufferPool<U>& pool, size_t size, std::unique_ptr<U>&& buffer)
            : pool(pool)
            , size(size)
            , buffer(std::move(buffer)) {}

        ~BufferPoolDeleter() {}
        BufferPoolDeleter(const BufferPoolDeleter&) = delete;
        BufferPoolDeleter& operator=(const BufferPoolDeleter&) = delete;
        BufferPoolDeleter(BufferPoolDeleter&&) = default;
        BufferPoolDeleter& operator=(BufferPoolDeleter&&) = default;

        void operator()(U*) { pool.release(size, std::move(buffer)); }
    };
private:
    mutable std::mutex mMutex;
    const size_t mCapacity;    // 总容量上限（字节数，剩余容量 = 总容量 - 已分配的容量）
    size_t mAllocatedCapacity; // 已分配的容量（字节数）= 池中 + 已使用容量（字节数）
    size_t mUsedCapacity;      // 已使用容量（字节数）
    std::map<size_t, std::queue<std::unique_ptr<T>>> mPools;
};

template <typename T>
template <typename... Args>
std::pair<std::shared_ptr<T>, bool> BufferPool<T>::acquire(size_t size, Args&&... args) {
    std::lock_guard<std::mutex> lock(mMutex);
    auto it = mPools.find(size);
    if (it != mPools.end() && !it->second.empty()) {
        auto buffer = std::move(it->second.front());
        it->second.pop();
        mUsedCapacity += size;
        auto ptr = std::shared_ptr<T>(buffer.get(), BufferPoolDeleter<T>{*this, size, std::move(buffer)});
        return {ptr, true};
    }

    size_t remaining = mCapacity - mAllocatedCapacity;
    if (size <= remaining) {
        auto buffer = std::make_unique<T>(std::forward<Args>(args)...);
        mAllocatedCapacity += size;
        mUsedCapacity += size;
        auto ptr = std::shared_ptr<T>(buffer.get(), BufferPoolDeleter<T>{*this, size, std::move(buffer)});
        return {ptr, false};
    }

    return {nullptr, false};
}

template <typename T>
void BufferPool<T>::release(size_t size, std::unique_ptr<T>&& buffer) {
    std::lock_guard<std::mutex> lock(mMutex);
    if (buffer) {
        mPools[size].push(std::move(buffer));
        mUsedCapacity -= size;
    }
}
