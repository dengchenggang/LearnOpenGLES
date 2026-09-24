#include "video/VideoCapture.h"
#include "VideoPipelineCamera.h"
#include "VideoPipelineImage.h"
#include "utils/Log.h"
#include <sstream>

#undef VideoCapture

namespace framework {

std::vector<std::string> split(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(s);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }
    return tokens;
}

bool VideoCapture::setVideoPipelineInfo(const std::string& url, int32_t width, int32_t height, VideoFormat format, float fps) {
    LogI("enter: setVideoPipelineInfo: url=%s, width=%d, height=%d, format=%d, fps=%f", url.c_str(), width, height, format, fps);
    std::lock_guard<std::mutex> lock(mMutex);
    auto result = mVideoPipelineInfo.emplace(url, VideoPipelineInfo {url, width, height, format, fps});
    LogI("exit: result=%d", result.second);
    return result.second;
}

void VideoCapture::setPipelineNotification(VideoPipelineNotification notification) {
    std::lock_guard<std::mutex> lock(mPipelineNotificationMutex);
    mPipelineNotification = std::move(notification);
}

bool VideoCapture::restart(const std::string& url, bool hardRestart) {
    LOG_ENTER("url=%s, hardRestart=%d", url.c_str(), hardRestart);
    std::lock_guard<std::mutex> lock(mMutex);

    auto it = mVideoPipelines.find(url);
    if (it == mVideoPipelines.end()) {
        LOG_EXIT("pipeline for %s not found", url.c_str());
        return false;
    }

    it->second->restart(hardRestart);
    LOG_EXIT("url=%s", url.c_str());
    return true;
}

bool VideoCapture::connect(const std::string& url, const std::string& moduleName, const VideoFrameCallback& callback) {
    LOG_ENTER("url=%s, moduleName=%s", url.c_str(), moduleName.c_str());
    std::lock_guard<std::mutex> lock(mMutex);

    auto pipeline = getOrCreatePipeline(url, false);
    if (!pipeline) {
        LogE("exit: failed to get or create pipeline for url: %s", url.c_str());
        return false;
    }

    pipeline->resume();

    auto result = pipeline->connect(moduleName, callback);
    LOG_EXIT("url=%s, moduleName=%s, %zu ?= %zu", url.c_str(), moduleName.c_str(), result.first, result.second);
    return result.first != result.second;
}

bool VideoCapture::connect(const std::string& url, const std::string& moduleName, const VideoHardwareBufferCallback& callback) {
    LOG_ENTER("url=%s, moduleName=%s", url.c_str(), moduleName.c_str());
    std::lock_guard<std::mutex> lock(mMutex);

    auto pipeline = getOrCreatePipeline(url, true);
    if (!pipeline) {
        LogE("exit: failed to get or create pipeline for url: %s", url.c_str());
        return false;
    }

    pipeline->resume();

    auto result = pipeline->connect(moduleName, callback);
    LOG_EXIT("url=%s, moduleName=%s, %zu ?= %zu", url.c_str(), moduleName.c_str(), result.first, result.second);
    return result.first != result.second;
}

bool VideoCapture::disconnect(const std::string& url, const std::string& moduleName, bool releaseIfNoObserver) {
    LOG_ENTER("url=%s, moduleName=%s", url.c_str(), moduleName.c_str());
    std::lock_guard<std::mutex> lock(mMutex);

    auto it = mVideoPipelines.find(url);
    if (it == mVideoPipelines.end()) {
        LOG_EXIT("pipeline for %s not found", url.c_str());
        return false;
    }

    auto result = it->second->disconnect(moduleName);
    if (result.second == 0) {
        if (releaseIfNoObserver) {
            it->second->stop();
            mVideoPipelines.erase(it);
        } else {
            it->second->pause();
        }
    }

    LOG_EXIT("url=%s, moduleName=%s, %zu ?= %zu", url.c_str(), moduleName.c_str(), result.first, result.second);
    return result.first != result.second;
}

VideoPipeline* VideoCapture::getOrCreatePipeline(const std::string& url, bool useHardwareBuffer) {
    auto it = mVideoPipelines.find(url);
    if (it != mVideoPipelines.end()) {
        if (it->second->useHardwareBuffer() != useHardwareBuffer) {
            LogE("pipeline for url %s uses a different hardware buffer mode", url.c_str());
            return nullptr;
        }
        return it->second.get();
    }

    auto infoIt = mVideoPipelineInfo.find(url);
    if (infoIt == mVideoPipelineInfo.end()) {
        LogE("no pipeline info for url: %s", url.c_str());
        return nullptr;
    }
    const auto& info = infoIt->second;

    std::unique_ptr<VideoPipeline> pipeline;
    try {
        if (url.find("camera://") == 0) {
            auto parts = split(url.substr(9), '/');
            if (parts.empty()) {
                LogE("invalid camera url: %s", url.c_str());
                return nullptr;
            }
            int32_t cameraId = std::stoi(parts[0]);
            pipeline = std::make_unique<VideoPipelineCamera>(cameraId, info.width, info.height, info.format, info.fps, useHardwareBuffer);
        } else if (url.find("image://") == 0) {
            std::string assetName = url.substr(8);
            pipeline = std::make_unique<VideoPipelineImage>(assetName, info.width, info.height, info.format, info.fps, useHardwareBuffer);
        } else {
            LogE("unsupported url scheme: %s", url.c_str());
            return nullptr;
        }
    } catch (...) {
        LogE("failed to parse url: %s", url.c_str());
        return nullptr;
    }

    if (!pipeline) {
        return nullptr;
    }

    pipeline->setNotification([this, url](VideoPipelineEvent event, int32_t errorCode, const std::string& message) {
        std::string pipelineMessage = url + ": " + message;
        VideoPipelineNotification notification;
        {
            std::lock_guard<std::mutex> lock(mPipelineNotificationMutex);
            notification = mPipelineNotification;
        }
        if (notification) {
            notification(url, event, errorCode, pipelineMessage);
        }
    });
    pipeline->start();
    auto result = mVideoPipelines.emplace(url, std::move(pipeline));
    return result.first->second.get();
}

VideoCapture::VideoCapture() = default;

VideoCapture::~VideoCapture() = default;

VideoCapture& GetVideoCaptureInstance() {
    return Singleton<VideoCapture>::getInstance();
}

} // namespace framework
