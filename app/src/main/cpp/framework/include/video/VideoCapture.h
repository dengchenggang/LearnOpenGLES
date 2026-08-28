#pragma once
#include "common/Singleton.hpp"
#include "video/VideoFrame.h"
#include "video/VideoHardwareBuffer.h"
#include <string>
#include <map>
#include <memory>
#include <mutex>

namespace framework {

class VideoPipeline;

class VideoCapture {
    friend class Singleton<VideoCapture>;
public:
    VideoCapture(const VideoCapture&) = delete;
    VideoCapture& operator=(const VideoCapture&) = delete;
public:
    bool connect(const std::string& url, const std::string& moduleName, const VideoFrameCallback& callback);
    bool connect(const std::string& url, const std::string& moduleName, const VideoHardwareBufferCallback& callback);
    bool disconnect(const std::string& url, const std::string& moduleName);
private:
    VideoCapture();
    ~VideoCapture();
private:
    std::map<std::string, std::unique_ptr<VideoPipeline>> mVideoPipelines;
    std::mutex mMutex;
};

VideoCapture& GetVideoCaptureInstance();

} // namespace framework

#define VideoCapture framework::GetVideoCaptureInstance()