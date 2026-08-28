#ifndef IMAGE_DATA_H
#define IMAGE_DATA_H

#include "FileData.h"
#include <cstdint>

namespace framework {

enum class EImageDataFormat {
    RGB_888,
    RGBA_8888
};

struct ImageData : public FileData {
    const int32_t width;
    const int32_t height;
    const EImageDataFormat format;

    ImageData(FileDataBufferType&& buffer, size_t bufferSize, const int32_t width, const int32_t height, const EImageDataFormat format)
        : FileData(std::move(buffer), bufferSize)
        , width(width)
        , height(height)
        , format(format) {}
};

} // namespace framework

#endif // IMAGE_DATA_H
