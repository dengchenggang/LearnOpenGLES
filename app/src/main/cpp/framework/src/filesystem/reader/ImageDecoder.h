#ifndef IMAGE_DECODER_H
#define IMAGE_DECODER_H

#include "ImageData.h"
#include <memory>
#include <cstring>

namespace framework {

class ImageDecoder {
public:
    static bool isImageFile(const char* filePath);
    static std::unique_ptr<ImageData> decode(const uint8_t* data, size_t size);
};

} // namespace framework

#endif
