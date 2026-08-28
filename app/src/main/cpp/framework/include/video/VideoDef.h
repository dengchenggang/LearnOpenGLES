#pragma once

#include <cstdint>

namespace framework {

enum class VideoFormat : int32_t {
    RGBA_8888 = 1,    // AIMAGE_FORMAT_RGBA_8888
    RGB_888 = 3,      // AIMAGE_FORMAT_RGB_888
    YUV_420_888 = 35, // AIMAGE_FORMAT_YUV_420_888
    Unknown = 0
};

inline size_t calculateSize(int32_t width, int32_t height, VideoFormat format) {
    switch (format) {
        case VideoFormat::RGBA_8888:
            return static_cast<size_t>(width) * height * 4;
        case VideoFormat::RGB_888:
            return static_cast<size_t>(width) * height * 3;
        case VideoFormat::YUV_420_888:
            return static_cast<size_t>(width) * height * 3 / 2;
        default:
            return static_cast<size_t>(width) * height * 4;
    }
}

} // namespace framework
