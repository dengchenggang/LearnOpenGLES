#pragma once
#include "VideoPipeline.h"
#include "Singleton.hpp"
#include <string>
#include <map>
#include <memory>
#include <mutex>

namespace framework {
namespace video {

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
    VideoCapture() = default;
    ~VideoCapture() = default;
private:
    std::map<std::string, std::unique_ptr<VideoPipeline>> mVideoPipelines;
    std::mutex mMutex;
};

} // namespace video
} // namespace framework

#define VideoCapture Singleton<framework::video::VideoCapture>::getInstance()