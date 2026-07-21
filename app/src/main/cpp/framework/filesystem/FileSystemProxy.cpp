#include "FileSystem.h"

void FileSystemProxy::setReader(std::unique_ptr<IFileReader> reader) {
    mReaders.push_back(std::move(reader));
}

FileData FileSystemProxy::readFile(const char* filePath) {
    for (const auto& reader : mReaders) {
        if (reader) {
            FileData data = reader->readFile(filePath);
            if (!data.empty()) {
                return data;
            }
        }
    }
    return FileData();
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
