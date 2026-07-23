#include "FileSystem.h"
#include "AssetManagerReader.h"
#include "FileReader.h"

void FileSystemProxy::SetReader(void* context) {
    mReaders.push_back(std::make_unique<AssetManagerReader>(context));
}

void FileSystemProxy::SetReader(const std::string& rootPath) {
    mReaders.push_back(std::make_unique<FileReader>(rootPath.c_str()));
}

bool FileSystemProxy::SetWriter(const std::string& rootPath) {
    mWriter = std::make_unique<FileWriter>(rootPath);
    return mWriter != nullptr;
}

std::unique_ptr<FileData> FileSystemProxy::readFile(const char* filePath) {
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

std::string FileSystemProxy::readString(const char* filePath) {
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

bool FileSystemProxy::exists(const char* filePath) {
    for (const auto& reader : mReaders) {
        if (reader && reader->exists(filePath)) {
            return true;
        }
    }
    return false;
}

size_t FileSystemProxy::getFileSize(const char* filePath) {
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
