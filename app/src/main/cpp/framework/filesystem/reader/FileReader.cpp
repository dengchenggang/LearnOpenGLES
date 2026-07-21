#include "FileReader.h"
#include <filesystem>
#include <fstream>
#include <limits>
#include <functional>

namespace fs = std::filesystem;

std::string FileReader::buildFullPath(const char* filePath) const {
    if (mRootPath.empty()) {
        return std::string(filePath);
    }
    return (fs::path(mRootPath) / filePath).string();
}

FileData FileReader::readFile(const char* filePath) {
    FileData result;

    std::lock_guard<std::mutex> lock(mMutex);
    if (!filePath) {
        return result;
    }

    std::string fullPath = buildFullPath(filePath);

    // 检查文件是否存在
    if (!fs::exists(fullPath) || !fs::is_regular_file(fullPath)) {
        return result;
    }

    // 获取文件大小
    auto fileSize = fs::file_size(fullPath);
    if (fileSize == 0 || fileSize > static_cast<uintmax_t>(std::numeric_limits<size_t>::max())) {
        return result;
    }

    // 打开文件并读取
    std::ifstream file(fullPath, std::ios::binary);
    if (!file.is_open()) {
        return result;
    }

    result.bufferSize = static_cast<size_t>(fileSize);
    result.buffer = std::unique_ptr<std::uint8_t[], std::function<void(std::uint8_t*)>>(
        new std::uint8_t[result.bufferSize],
        [](std::uint8_t* p) { delete[] p; }
    );
    file.read(reinterpret_cast<char*>(result.buffer.get()), static_cast<std::streamsize>(fileSize));

    if (!file) {
        result.buffer.reset();
        result.bufferSize = 0;
    }

    return result;
}

std::string FileReader::readString(const char* filePath) {
    std::lock_guard<std::mutex> lock(mMutex);
    if (!filePath) {
        return "";
    }

    std::string fullPath = buildFullPath(filePath);
    if (!fs::exists(fullPath) || !fs::is_regular_file(fullPath)) {
        return "";
    }

    std::ifstream file(fullPath);
    if (!file.is_open()) {
        return "";
    }

    return std::string((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
}

bool FileReader::exists(const char* filePath) {
    std::lock_guard<std::mutex> lock(mMutex);
    if (!filePath) {
        return false;
    }

    std::string fullPath = buildFullPath(filePath);
    return fs::exists(fullPath) && fs::is_regular_file(fullPath);
}

size_t FileReader::getFileSize(const char* filePath) {
    std::lock_guard<std::mutex> lock(mMutex);
    if (!filePath) {
        return 0;
    }

    std::string fullPath = buildFullPath(filePath);
    if (!fs::exists(fullPath) || !fs::is_regular_file(fullPath)) {
        return 0;
    }

    auto size = fs::file_size(fullPath);
    if (size > static_cast<uintmax_t>(std::numeric_limits<size_t>::max())) {
        return 0;
    }

    return static_cast<size_t>(size);
}

void FileReader::setRootPath(const char* rootPath) {
    std::lock_guard<std::mutex> lock(mMutex);
    if (rootPath) {
        mRootPath = rootPath;
    } else {
        mRootPath.clear();
    }
}
