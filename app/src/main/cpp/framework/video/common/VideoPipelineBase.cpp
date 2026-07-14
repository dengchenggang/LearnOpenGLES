#include "VideoPipelineBase.h"

std::pair<size_t, size_t> VideoPipelineBase::connect(const std::string& moduleName, VideoFrameCallback callback) {
    std::unique_lock<std::shared_mutex> lock(mConnectionsMutex);

    size_t oldSize = mConnections.size();

    if (mConnections.find(moduleName) != mConnections.end()) {
        return {oldSize, oldSize};
    }

    Connection conn;
    conn.callback = callback;

    mConnections.emplace(moduleName, std::move(conn));

    return {oldSize, mConnections.size()};
}

std::pair<size_t, size_t> VideoPipelineBase::disconnect(const std::string& moduleName) {
    std::unique_lock<std::shared_mutex> lock(mConnectionsMutex);

    size_t oldSize = mConnections.size();

    mConnections.erase(moduleName);

    return {oldSize, mConnections.size()};
}

void VideoPipelineBase::dispath(const VideoFramePtr& videoFrame) {
    if (!videoFrame || !videoFrame->frame) {
        return;
    }

    std::unique_lock<std::shared_mutex> lock(mConnectionsMutex);

    for (auto& [name, conn] : mConnections) {
        if (conn.callback) {
            conn.callback(videoFrame);
        }
    }
}
