#include "VideoCapture.h"
#include "VideoPipelineCamera.h"
#include "VideoPipelineImage.h"
#include "Log.h"
#include <sstream>

namespace framework {
namespace video {

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

VideoFormat parseFormat(const std::string& s) {
    if (s == "RGBA_8888") return VideoFormat::RGBA_8888;
    if (s == "RGB_888")   return VideoFormat::RGB_888;
    if (s == "YUV_420_888") return VideoFormat::YUV_420_888;
    return VideoFormat::RGBA_8888;
}

std::unique_ptr<VideoPipeline> createPipeline(const std::string& url) {
    try {
        if (url.find("camera://") == 0) {
            auto parts = split(url.substr(9), '/');
            if (parts.size() < 4) {
                LogE("invalid camera url: %s", url.c_str());
                return nullptr;
            }
            int32_t cameraId = std::stoi(parts[0]);
            int32_t width    = std::stoi(parts[1]);
            int32_t height   = std::stoi(parts[2]);
            VideoFormat format = parseFormat(parts[3]);
            return std::make_unique<VideoPipelineCamera>(cameraId, width, height, format);
        }

        if (url.find("image://") == 0) {
            auto parts = split(url.substr(8), '/');
            if (parts.size() < 5) {
                LogE("invalid image url: %s", url.c_str());
                return nullptr;
            }
            std::string assetName = parts[0];
            int width  = std::stoi(parts[1]);
            int height = std::stoi(parts[2]);
            VideoFormat format = parseFormat(parts[3]);
            float fps = std::stof(parts[4]);
            return std::make_unique<VideoPipelineImage>(assetName, width, height, format, fps);
        }
    } catch (...) {
        LogE("failed to parse url: %s", url.c_str());
        return nullptr;
    }

    LogE("unsupported url scheme: %s", url.c_str());
    return nullptr;
}

bool VideoCapture::connect(const std::string& url, const std::string& moduleName, const VideoFrameCallback& callback) {
    LOG_ENTER("url=%s, moduleName=%s", url.c_str(), moduleName.c_str());
    std::lock_guard<std::mutex> lock(mMutex);

    auto it = mVideoPipelines.find(url);
    if (it == mVideoPipelines.end()) {
        auto pipeline = createPipeline(url);
        if (!pipeline) {
            LogE("exit: failed to create pipeline for url: %s", url.c_str());
            return false;
        }
        it = mVideoPipelines.emplace(url, std::move(pipeline)).first;
        it->second->start();
    }

    auto result = it->second->connect(moduleName, callback);
    LOG_EXIT("url=%s, moduleName=%s, %zu ?= %zu", url.c_str(), moduleName.c_str(), result.first, result.second);
    return result.first != result.second;
}

bool VideoCapture::connect(const std::string& url, const std::string& moduleName, const VideoHardwareBufferCallback& callback) {
    LOG_ENTER("url=%s, moduleName=%s", url.c_str(), moduleName.c_str());
    std::lock_guard<std::mutex> lock(mMutex);

    auto it = mVideoPipelines.find(url);
    if (it == mVideoPipelines.end()) {
        auto pipeline = createPipeline(url);
        if (!pipeline) {
            LogE("exit: failed to create pipeline for url: %s", url.c_str());
            return false;
        }
        it = mVideoPipelines.emplace(url, std::move(pipeline)).first;
        it->second->start();
    }

    auto result = it->second->connect(moduleName, callback);
    LOG_EXIT("url=%s, moduleName=%s, %zu ?= %zu", url.c_str(), moduleName.c_str(), result.first, result.second);
    return result.first != result.second;
}

bool VideoCapture::disconnect(const std::string& url, const std::string& moduleName) {
    LOG_ENTER("url=%s, moduleName=%s", url.c_str(), moduleName.c_str());
    std::lock_guard<std::mutex> lock(mMutex);

    auto it = mVideoPipelines.find(url);
    if (it == mVideoPipelines.end()) {
        LOG_EXIT("pipeline for %s not found", url.c_str());
        return false;
    }

    auto result = it->second->disconnect(moduleName);
    if (result.second == 0) {
        it->second->stop();
        mVideoPipelines.erase(it);
    }

    LOG_EXIT("url=%s, moduleName=%s, %zu ?= %zu", url.c_str(), moduleName.c_str(), result.first, result.second);
    return result.first != result.second;
}

} // namespace video
} // namespace framework
