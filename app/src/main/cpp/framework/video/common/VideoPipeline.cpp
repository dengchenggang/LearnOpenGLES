#include "VideoPipeline.h"

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

void VideoPipeline::dispath(const VideoFramePtr& videoFrame) {
    if (!videoFrame) {
        return;
    }

    std::unique_lock<std::shared_mutex> lock(mConnectionsMutex);

    for (auto& [name, callback] : mConnections) {
        if (callback) {
            callback(videoFrame);
        }
    }
}

void VideoPipeline::dispath(const VideoHardwareBufferPtr& hardwareBuffer) {
    if (!hardwareBuffer) {
        return;
    }

    std::unique_lock<std::shared_mutex> lock(mConnectionsMutex);

    for (auto& [name, callback] : mHardwareBufferConnections) {
        if (callback) {
            callback(hardwareBuffer);
        }
    }
}
