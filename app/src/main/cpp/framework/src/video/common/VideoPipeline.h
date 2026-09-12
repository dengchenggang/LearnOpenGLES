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
    explicit VideoPipeline(bool useHardwareBuffer);
    virtual ~VideoPipeline();
    VideoPipeline(const VideoPipeline&) = delete;
    VideoPipeline& operator = (const VideoPipeline&) = delete;
public:

    virtual void start() = 0;
    virtual void stop() = 0;
    void restart();

    std::pair<size_t, size_t> connect(const std::string& moduleName, VideoFrameCallback callback);
    std::pair<size_t, size_t> connect(const std::string& moduleName, VideoHardwareBufferCallback callback);
    std::pair<size_t, size_t> disconnect(const std::string& moduleName);

    bool useHardwareBuffer() const { return mUseHardwareBuffer; }
    void setNotification(VideoPipelineNotification notification) {
        std::lock_guard<std::mutex> lock(mNotificationMutex);
        mNotification = std::move(notification);
    }
protected:
    void notify(VideoPipelineState state, int32_t errorCode = 0, std::string message = {});
    void notifyFirstFrame();
    void resetFirstFrameNotification();
    void dispath(const VideoFramePtr&);
    void dispath(const VideoHardwareBufferPtr&);
private:
    const bool mUseHardwareBuffer;
    std::map<std::string, VideoFrameCallback> mConnections;
    std::map<std::string, VideoHardwareBufferCallback> mHardwareBufferConnections;
    mutable std::shared_mutex mConnectionsMutex;
    VideoPipelineNotification mNotification;
    mutable std::mutex mNotificationMutex;
    std::unique_ptr<TaskPool> mNotificationTaskPool;
    std::atomic<bool> mFirstFrameNotified {false};
};

} // namespace framework

#endif
