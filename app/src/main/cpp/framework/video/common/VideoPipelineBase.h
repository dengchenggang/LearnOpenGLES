#ifndef C_VIDEO_PIPELINE_BASE_H
#define C_VIDEO_PIPELINE_BASE_H
#include "VideoFrame.h"
#include <functional>
#include <map>
#include <shared_mutex>
#include <string>

using VideoFrameCallback = std::function<void(const VideoFramePtr&)>;

class VideoPipelineBase {
public:
    virtual ~VideoPipelineBase() = default;
    VideoPipelineBase(const VideoPipelineBase&) = delete;
    VideoPipelineBase& operator = (const VideoPipelineBase&) = delete;
public:
    std::pair<size_t, size_t> connect(const std::string& moduleName, VideoFrameCallback callback);
    std::pair<size_t, size_t> disconnect(const std::string& moduleName);
protected:
    void dispath(const VideoFramePtr&);
private:
    struct Connection {
        VideoFrameCallback callback;
    };
private:
    std::map<std::string, Connection> mConnections;
    mutable std::shared_mutex mConnectionsMutex;
};
#endif
