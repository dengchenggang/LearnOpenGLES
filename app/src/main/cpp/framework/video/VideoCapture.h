#pragma once
#include "VideoPipelineBase.h"
#include <string>
#include <map>
#include <memory>
#include <mutex>

class VideoCapture {
public:
    VideoCapture() = default;
    ~VideoCapture() = default;
    VideoCapture(const VideoCapture&) = delete;
    VideoCapture& operator=(const VideoCapture&) = delete;
public:
    bool connect(const std::string& url, const std::string& moduleName, const VideoFrameCallback& callback);
    bool connect(const std::string& url, const std::string& moduleName, const VideoHardwareBufferCallback& callback);
    bool disconnect(const std::string& url, const std::string& moduleName);
private:
    std::map<std::string, std::unique_ptr<VideoPipelineBase>> mVideoPipelines;
    std::mutex mMutex;
};
