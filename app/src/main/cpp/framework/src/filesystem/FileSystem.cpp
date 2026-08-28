#include "FileSystem.h"
#include "FileWriter.h"
#include "AssetManagerReader.h"
#include "FileReader.h"
#include "Log.h"

#undef FileSystem

namespace framework {

FileSystem::FileSystem() = default;
FileSystem::~FileSystem() = default;

void FileSystem::SetReader(void* context) {
    mReaders.push_back(std::make_unique<AssetManagerReader>(context));
}

void FileSystem::SetReader(const std::string& rootPath) {
    LogI("rootPath=%s", rootPath.c_str());
    mReaders.push_back(std::make_unique<FileReader>(rootPath.c_str()));
}

bool FileSystem::SetWriter(const std::string& rootPath) {
    mWriter = std::make_unique<FileWriter>(rootPath);
    return mWriter != nullptr;
}

void FileSystem::reset() {
    mReaders.clear();
    mWriter.reset();
}

std::unique_ptr<FileData> FileSystem::readFile(const char* filePath) {
    for (const auto& reader : mReaders) {
        if (reader) {
            auto data = reader->readFile(filePath);
            if (data && !data->empty()) {
                return data;
            }
        }
    }
    return nullptr;
}

std::string FileSystem::readString(const char* filePath) {
    for (const auto& reader : mReaders) {
        if (reader) {
            std::string str = reader->readString(filePath);
            if (!str.empty()) {
                return str;
            }
        }
    }
    return "";
}

bool FileSystem::exists(const char* filePath) {
    for (const auto& reader : mReaders) {
        if (reader && reader->exists(filePath)) {
            return true;
        }
    }
    return false;
}

size_t FileSystem::getFileSize(const char* filePath) {
    for (const auto& reader : mReaders) {
        if (reader) {
            size_t size = reader->getFileSize(filePath);
            if (size > 0) {
                return size;
            }
        }
    }
    return 0;
}

} // namespace framework
