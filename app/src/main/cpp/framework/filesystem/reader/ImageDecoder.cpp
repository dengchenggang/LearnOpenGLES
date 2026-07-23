#include "ImageDecoder.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

bool ImageDecoder::isImageFile(const char* filePath) {
    if (!filePath) return false;
    const char* ext = std::strrchr(filePath, '.');
    if (!ext) return false;
    return std::strcmp(ext, ".png") == 0 ||
           std::strcmp(ext, ".jpg") == 0 ||
           std::strcmp(ext, ".jpeg") == 0 ||
           std::strcmp(ext, ".bmp") == 0 ||
           std::strcmp(ext, ".tga") == 0;
}

std::unique_ptr<ImageData> ImageDecoder::decode(const uint8_t* data, size_t size) {
    int width = 0, height = 0, channels = 0;
    uint8_t* pixels = stbi_load_from_memory(
        data, static_cast<int>(size),
        &width, &height, &channels, 0
    );
    if (!pixels) {
        return nullptr;
    }

    size_t bufferSize = static_cast<size_t>(width * height * channels);
    auto buffer = FileDataBufferType(
        pixels,
        [](std::uint8_t* p) { stbi_image_free(p); }
    );
    EImageDataFormat format = (channels == 4) ? EImageDataFormat::RGBA_8888 : EImageDataFormat::RGB_888;
    return std::make_unique<ImageData>(std::move(buffer), bufferSize, width, height, format);
}
