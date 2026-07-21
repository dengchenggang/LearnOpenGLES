#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"
#include "ImageWriter.h"

bool ImageWriter::writePNG(const char* filename, int width, int height, int channels, const uint8_t* data) {
    return stbi_write_png(filename, width, height, channels, data, width * channels) != 0;
}

bool ImageWriter::writeJPG(const char* filename, int width, int height, int channels, const uint8_t* data, int quality) {
    return stbi_write_jpg(filename, width, height, channels, data, quality) != 0;
}

bool ImageWriter::writeBMP(const char* filename, int width, int height, int channels, const uint8_t* data) {
    return stbi_write_bmp(filename, width, height, channels, data) != 0;
}

bool ImageWriter::writeTGA(const char* filename, int width, int height, int channels, const uint8_t* data) {
    return stbi_write_tga(filename, width, height, channels, data) != 0;
}
