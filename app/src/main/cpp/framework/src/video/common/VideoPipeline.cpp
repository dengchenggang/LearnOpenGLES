#include "VideoPipeline.h"
#include <utility>

namespace framework {

VideoPipeline::VideoPipeline(bool useHardwareBuffer)
    : mUseHardwareBuffer(useHardwareBuffer)
    , mNotificationTaskPool(std::make_unique<TaskPool>())
{
    mNotificationTaskPool->start();
}

VideoPipeline::~VideoPipeline() {
    mNotificationTaskPool->stop();
}

void VideoPipeline::notify(VideoPipelineEvent event, int32_t errorCode, std::string message) {
    auto notification = [this]() {
        std::lock_guard<std::mutex> lock(mNotificationMutex);
        return mNotification;
    }();
    if (notification) {
        mNotificationTaskPool->detach(
            [notification = std::move(notification), event, errorCode, message = std::move(message)]() {
                notification(event, errorCode, message);
            }
        );
    }
}

std::pair<size_t, size_t> VideoPipeline::connect(const std::string& moduleName, VideoFrameCallback callback) {
    std::unique_lock<std::shared_mutex> lock(mConnectionsMutex);

    size_t oldSize = mConnections.size();

    auto it = mConnections.find(moduleName);
    if (it != mConnections.end()) {
        it->second = callback;
        return {oldSize, oldSize};
    }

    mConnections.emplace(moduleName, std::move(callback));

    return {oldSize, mConnections.size()};
}

std::pair<size_t, size_t> VideoPipeline::connect(const std::string& moduleName, VideoHardwareBufferCallback callback) {
    std::unique_lock<std::shared_mutex> lock(mConnectionsMutex);

    size_t oldSize = mHardwareBufferConnections.size();

    auto it = mHardwareBufferConnections.find(moduleName);
    if (it != mHardwareBufferConnections.end()) {
        it->second = callback;
        return {oldSize, oldSize};
    }

    mHardwareBufferConnections.emplace(moduleName, std::move(callback));

    return {oldSize, mHardwareBufferConnections.size()};
}

std::pair<size_t, size_t> VideoPipeline::disconnect(const std::string& moduleName) {
    std::unique_lock<std::shared_mutex> lock(mConnectionsMutex);

    size_t oldFrameSize = mConnections.size();
    size_t oldHwSize = mHardwareBufferConnections.size();

    mConnections.erase(moduleName);
    mHardwareBufferConnections.erase(moduleName);

    size_t newSize = mConnections.size() + mHardwareBufferConnections.size();
    size_t oldSize = oldFrameSize + oldHwSize;

    return {oldSize, newSize};
}

void VideoPipeline::dispatch(const VideoFramePtr& videoFrame) {
    if (!videoFrame) {
        return;
    }

    std::shared_lock<std::shared_mutex> lock(mConnectionsMutex);
    for (auto& pair : mConnections) {
        auto& callback = pair.second;
        if (callback) {
            mNotificationTaskPool->detach([callback, videoFrame]() {
                callback(videoFrame);
            });
        }
    }
}

void VideoPipeline::dispatch(const VideoHardwareBufferPtr& hardwareBuffer) {
    if (!hardwareBuffer) {
        return;
    }

    std::shared_lock<std::shared_mutex> lock(mConnectionsMutex);
    for (auto& pair : mHardwareBufferConnections) {
        auto& callback = pair.second;
        if (callback) {
            mNotificationTaskPool->detach([callback, hardwareBuffer]() {
                callback(hardwareBuffer);
            });
        }
    }
}

} // namespace framework
