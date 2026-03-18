#include "FileSystem.h"

void FileSystemProxy::setReader(std::unique_ptr<IFileReader> reader) {
    mReader = std::move(reader);
}

IFileReader* FileSystemProxy::getReader() const {
    return mReader.get();
}

FileData FileSystemProxy::readFile(const char* filePath) {
    if (mReader) {
        return mReader->readFile(filePath);
    }
    return FileData();
}

std::string FileSystemProxy::readString(const char* filePath) {
    if (mReader) {
        return mReader->readString(filePath);
    }
    return "";
}

bool FileSystemProxy::exists(const char* filePath) {
    if (mReader) {
        return mReader->exists(filePath);
    }
    return false;
}

size_t FileSystemProxy::getFileSize(const char* filePath) {
    if (mReader) {
        return mReader->getFileSize(filePath);
    }
    return 0;
}
