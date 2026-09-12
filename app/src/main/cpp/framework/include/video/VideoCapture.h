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
    bool setVideoPipelineInfo(const std::string& url, int32_t width, int32_t height, VideoFormat format, float fps);
    void setPipelineNotification(VideoPipelineNotification notification);
    bool restart(const std::string& url);
    bool connect(const std::string& url, const std::string& moduleName, const VideoFrameCallback& callback);
    bool connect(const std::string& url, const std::string& moduleName, const VideoHardwareBufferCallback& callback);
    bool disconnect(const std::string& url, const std::string& moduleName);
private:
    struct VideoPipelineInfo {
        std::string url;
        int32_t width;
        int32_t height;
        VideoFormat format;
        float fps;
    };
private:
    VideoPipeline* getOrCreatePipeline(const std::string& url, bool useHardwareBuffer);

    VideoCapture();
    ~VideoCapture();
private:
    VideoPipelineNotification mPipelineNotification;
    std::mutex mPipelineNotificationMutex;
    std::map<std::string, VideoPipelineInfo> mVideoPipelineInfo;
    std::map<std::string, std::unique_ptr<VideoPipeline>> mVideoPipelines;
    std::mutex mMutex;
};

VideoCapture& GetVideoCaptureInstance();

} // namespace framework

#define VideoCapture framework::GetVideoCaptureInstance()