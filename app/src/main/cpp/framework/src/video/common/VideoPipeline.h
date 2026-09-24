#ifndef C_VIDEO_PIPELINE_H
#define C_VIDEO_PIPELINE_H
#include "video/VideoFrame.h"
#include "video/VideoHardwareBuffer.h"
#include "utils/TaskPool.h"
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <atomic>
#include <shared_mutex>
#include <string>

namespace framework {

class VideoPipeline {
public:
    enum class State { Idle, Started, Resumed, Paused };
    using Notification = std::function<void(VideoPipelineEvent, int32_t, const std::string&)>;
public:
    explicit VideoPipeline(bool useHardwareBuffer);
    virtual ~VideoPipeline();
    VideoPipeline(const VideoPipeline&) = delete;
    VideoPipeline& operator = (const VideoPipeline&) = delete;
public:
    virtual void start() = 0;
    virtual void resume() = 0;
    virtual void pause() = 0;
    virtual void stop() = 0;
    virtual void restart(bool hardRestart = true) = 0;
public:
    std::pair<size_t, size_t> connect(const std::string& moduleName, VideoFrameCallback callback);
    std::pair<size_t, size_t> connect(const std::string& moduleName, VideoHardwareBufferCallback callback);
    std::pair<size_t, size_t> disconnect(const std::string& moduleName);

    bool useHardwareBuffer() const { return mUseHardwareBuffer; }
    State getState() const { return mState.load(); }
    State getPreviousState() const { return mStatePrevious.load(); }
    size_t getConnectionCount() const {
        std::shared_lock<std::shared_mutex> lock(mConnectionsMutex);
        return mConnections.size() + mHardwareBufferConnections.size();
    }
    void setNotification(Notification notification) {
        std::lock_guard<std::mutex> lock(mNotificationMutex);
        mNotification = std::move(notification);
    }
protected:
    void setState(State state) {
        mStatePrevious.store(mState.load());
        mState.store(state);
    }
    void notify(VideoPipelineEvent event, int32_t errorCode = 0, std::string message = {});
    void dispatch(const VideoFramePtr&);
    void dispatch(const VideoHardwareBufferPtr&);
private:
    const bool mUseHardwareBuffer;
    std::map<std::string, VideoFrameCallback> mConnections;
    std::map<std::string, VideoHardwareBufferCallback> mHardwareBufferConnections;
    mutable std::shared_mutex mConnectionsMutex;
    Notification mNotification;
    mutable std::mutex mNotificationMutex;
    std::unique_ptr<TaskPool> mNotificationTaskPool;
    std::atomic<State> mState{State::Idle};
    std::atomic<State> mStatePrevious{State::Idle};
};

} // namespace framework

#endif
