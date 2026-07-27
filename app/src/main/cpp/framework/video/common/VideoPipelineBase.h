#ifndef C_VIDEO_PIPELINE_BASE_H
#define C_VIDEO_PIPELINE_BASE_H
#include "VideoFrame.h"
#include "VideoHardwareBuffer.h"
#include <functional>
#include <map>
#include <shared_mutex>
#include <string>

using VideoFrameCallback = std::function<void(const VideoFramePtr&)>;
using VideoHardwareBufferCallback = std::function<void(const VideoHardwareBufferPtr&)>;

class VideoPipelineBase {
public:
    VideoPipelineBase() = default;
    virtual ~VideoPipelineBase() = default;
    VideoPipelineBase(const VideoPipelineBase&) = delete;
    VideoPipelineBase& operator = (const VideoPipelineBase&) = delete;
public:
    virtual void start() = 0;
    virtual void stop() = 0;

    std::pair<size_t, size_t> connect(const std::string& moduleName, VideoFrameCallback callback);
    std::pair<size_t, size_t> connect(const std::string& moduleName, VideoHardwareBufferCallback callback);
    std::pair<size_t, size_t> disconnect(const std::string& moduleName);
protected:
    void dispath(const VideoFramePtr&);
    void dispath(const VideoHardwareBufferPtr&);
private:
    std::map<std::string, VideoFrameCallback> mConnections;
    std::map<std::string, VideoHardwareBufferCallback> mHardwareBufferConnections;
    mutable std::shared_mutex mConnectionsMutex;
};
#endif
