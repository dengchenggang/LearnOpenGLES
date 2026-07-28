#ifndef C_VIDEO_PIPELINE_H
#define C_VIDEO_PIPELINE_H
#include "VideoFrame.h"
#include "VideoHardwareBuffer.h"
#include <functional>
#include <map>
#include <shared_mutex>
#include <string>

using VideoFrameCallback = std::function<void(const VideoFramePtr&)>;
using VideoHardwareBufferCallback = std::function<void(const VideoHardwareBufferPtr&)>;

class VideoPipeline {
public:
    VideoPipeline() = default;
    virtual ~VideoPipeline() = default;
    VideoPipeline(const VideoPipeline&) = delete;
    VideoPipeline& operator = (const VideoPipeline&) = delete;
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
