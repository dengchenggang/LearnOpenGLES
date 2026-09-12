#pragma once

#include <cstdint>
#include <functional>
#include <string>

namespace framework {

enum class VideoFormat : uint32_t {
    RGBA_8888 = 1,    // AIMAGE_FORMAT_RGBA_8888
    RGB_888 = 3,      // AIMAGE_FORMAT_RGB_888
    YUV_420_888 = 35, // AIMAGE_FORMAT_YUV_420_888
    Unknown = 0
};

enum class VideoPipelineState : uint32_t {
    Starting,
    Running,
    FirstFrame,
    Stopped,
    Error
};

using VideoPipelineNotification = std::function<void(VideoPipelineState, int32_t, const std::string&)>;

inline VideoFormat toVideoFormat(const char* format) {
    if (strcmp(format, "RGBA_8888") == 0) {
        return VideoFormat::RGBA_8888;
    } else if (strcmp(format, "RGB_888") == 0) {
        return VideoFormat::RGB_888;
    } else if (strcmp(format, "YUV_420_888") == 0) {
        return VideoFormat::YUV_420_888;
    } else {
        return VideoFormat::Unknown;
    }
}

inline VideoFormat toVideoFormat(const std::string& format) {
    return toVideoFormat(format.c_str());
}

inline const char* toString(VideoFormat videoFormat) {
    switch (videoFormat) {
        case VideoFormat::RGBA_8888:
            return "RGBA_8888";
        case VideoFormat::RGB_888:
            return "RGB_888";
        case VideoFormat::YUV_420_888:
            return "YUV_420_888";
        default:
            return "Unknown";
    }
}

inline int32_t calculateStride(int32_t width, VideoFormat format) {
    switch (format) {
        case VideoFormat::RGBA_8888:
            return width * 4;
        case VideoFormat::RGB_888:
            return width * 3;
        case VideoFormat::YUV_420_888:
            return width;
        default:
            return width;
    }
}

inline int64_t calculateSize(int32_t width, int32_t height, VideoFormat format) {
    switch (format) {
        case VideoFormat::RGBA_8888:
            return width * height * 4;
        case VideoFormat::RGB_888:
            return width * height * 3;
        case VideoFormat::YUV_420_888:
            return width * height * 3 / 2;
        default:
            return width * height;
    }
}

} // namespace framework
