#include "FileReader.h"
#include <filesystem>
#include <fstream>
#include <limits>

namespace fs = std::filesystem;

void FileReader::initialize(void* context) {
    std::lock_guard<std::mutex> lock(mMutex);
    if (!mInitialized) {
        if (context) {
            mRootPath = static_cast<const char*>(context);
        }
        mInitialized = true;
    }
}

bool FileReader::isInitialized() const {
    std::lock_guard<std::mutex> lock(mMutex);
    return mInitialized;
}

std::string FileReader::buildFullPath(const char* filePath) const {
    if (mRootPath.empty()) {
        return std::string(filePath);
    }
    return (fs::path(mRootPath) / filePath).string();
}

FileData FileReader::readFile(const char* filePath) {
    FileData result;

    std::lock_guard<std::mutex> lock(mMutex);
    if (!mInitialized || !filePath) {
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

    result.data.resize(static_cast<size_t>(fileSize));
    file.read(reinterpret_cast<char*>(result.data.data()), static_cast<std::streamsize>(fileSize));

    if (!file) {
        result.data.clear();
    }

    return result;
}

std::string FileReader::readString(const char* filePath) {
    FileData data = readFile(filePath);
    return data.asString();
}

bool FileReader::exists(const char* filePath) {
    std::lock_guard<std::mutex> lock(mMutex);
    if (!mInitialized || !filePath) {
        return false;
    }

    std::string fullPath = buildFullPath(filePath);
    return fs::exists(fullPath) && fs::is_regular_file(fullPath);
}

size_t FileReader::getFileSize(const char* filePath) {
    std::lock_guard<std::mutex> lock(mMutex);
    if (!mInitialized || !filePath) {
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
