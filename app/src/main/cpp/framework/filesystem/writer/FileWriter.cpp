#include "FileWriter.h"
#include "ImageWriter.h"
#include <cstring>
#include <fstream>

namespace {
    enum class ImageFormat {
        PNG,
        JPG,
        BMP,
        TGA,
        Unknown
    };

    ImageFormat getImageFormat(const char* filePath) {
        if (!filePath) {
            return ImageFormat::Unknown;
        }
        const char* ext = std::strrchr(filePath, '.');
        if (!ext) {
            return ImageFormat::Unknown;
        }
        if (std::strcmp(ext, ".png") == 0) {
            return ImageFormat::PNG;
        }
        if (std::strcmp(ext, ".jpg") == 0 || std::strcmp(ext, ".jpeg") == 0) {
            return ImageFormat::JPG;
        }
        if (std::strcmp(ext, ".bmp") == 0) {
            return ImageFormat::BMP;
        }
        if (std::strcmp(ext, ".tga") == 0) {
            return ImageFormat::TGA;
        }
        return ImageFormat::Unknown;
    }

    std::string joinPath(const std::string& root, const std::string& path) {
        if (root.empty()) {
            return path;
        }
        if (root.back() == '/' || root.back() == '\\') {
            return root + path;
        }
        return root + "/" + path;
    }
}

FileWriter::FileWriter(const std::string& rootPath) : mRootPath(rootPath) {}

bool FileWriter::write(const std::string& filePath, const std::string& content) {
    return write(filePath, content.c_str(), content.length());
}

bool FileWriter::write(const std::string& filePath, const char* data, size_t length) {
    std::string fullPath = joinPath(mRootPath, filePath);
    std::ofstream ofs(fullPath, std::ios::out | std::ios::trunc);
    if (!ofs.is_open()) {
        return false;
    }
    ofs.write(data, static_cast<std::streamsize>(length));
    return ofs.good();
}

bool FileWriter::write(const std::string& filePath, const uint8_t* data, size_t size) {
    std::string fullPath = joinPath(mRootPath, filePath);
    std::ofstream ofs(fullPath, std::ios::out | std::ios::binary | std::ios::trunc);
    if (!ofs.is_open()) {
        return false;
    }
    ofs.write(reinterpret_cast<const char*>(data), static_cast<std::streamsize>(size));
    return ofs.good();
}

bool FileWriter::write(const std::string& filePath, int width, int height, int channels, const uint8_t* data, int quality) {
    std::string fullPath = joinPath(mRootPath, filePath);
    switch (getImageFormat(filePath.c_str())) {
        case ImageFormat::PNG:
            return ImageWriter::writePNG(fullPath.c_str(), width, height, channels, data);
        case ImageFormat::JPG:
            return ImageWriter::writeJPG(fullPath.c_str(), width, height, channels, data, quality);
        case ImageFormat::BMP:
            return ImageWriter::writeBMP(fullPath.c_str(), width, height, channels, data);
        case ImageFormat::TGA:
            return ImageWriter::writeTGA(fullPath.c_str(), width, height, channels, data);
        default:
            return false;
    }
}
