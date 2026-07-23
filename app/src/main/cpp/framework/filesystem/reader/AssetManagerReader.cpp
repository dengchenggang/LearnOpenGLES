#include "AssetManagerReader.h"
#include "ImageDecoder.h"
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include "Log.h"

constexpr const char* TAG {"AssetManagerReader"};

std::unique_ptr<FileData> AssetManagerReader::readFile(const char* filePath) {
    std::lock_guard<std::mutex> lock(mMutex);
    if (!mAssetManager || !filePath) {
        return nullptr;
    }

    AAssetManager* mgr = static_cast<AAssetManager*>(mAssetManager);
    AAsset* asset = AAssetManager_open(mgr, filePath, AASSET_MODE_BUFFER);
    if (!asset) {
        return nullptr;
    }

    off_t fileSize = AAsset_getLength(asset);
    if (fileSize <= 0) {
        AAsset_close(asset);
        return nullptr;
    }

    const void* assetBuffer = AAsset_getBuffer(asset);
    if (!assetBuffer) {
        AAsset_close(asset);
        return nullptr;
    }

    if (ImageDecoder::isImageFile(filePath)) {
        auto imageData = ImageDecoder::decode(static_cast<const uint8_t*>(assetBuffer), static_cast<size_t>(fileSize));
        AAsset_close(asset);
        return imageData;
    }

    auto buffer = FileDataBufferType(
        const_cast<std::uint8_t*>(static_cast<const std::uint8_t*>(assetBuffer)),
        [asset](std::uint8_t*) { AAsset_close(asset); }
    );
    return std::make_unique<FileData>(std::move(buffer), static_cast<size_t>(fileSize));
}

std::string AssetManagerReader::readString(const char* filePath) {
    std::lock_guard<std::mutex> lock(mMutex);
    if (!mAssetManager || !filePath) {
        return "";
    }

    AAssetManager* mgr = static_cast<AAssetManager*>(mAssetManager);
    AAsset* asset = AAssetManager_open(mgr, filePath, AASSET_MODE_STREAMING);
    if (!asset) {
        return "";
    }

    off_t fileSize = AAsset_getLength(asset);
    std::string result;
    if (fileSize > 0) {
        result.resize(static_cast<size_t>(fileSize));
        off_t bytesRead = AAsset_read(asset, result.data(), static_cast<size_t>(fileSize));
        if (bytesRead != fileSize) {
            result.clear();
        }
    }

    AAsset_close(asset);
    return result;
}

bool AssetManagerReader::exists(const char* filePath) {
    std::lock_guard<std::mutex> lock(mMutex);
    if (!mAssetManager || !filePath) {
        LogE("%s exists: filePath=%s or mAssetManager is nullptr", TAG, filePath);
        return false;
    }

    AAssetManager* mgr = static_cast<AAssetManager*>(mAssetManager);
    AAsset* asset = AAssetManager_open(mgr, filePath, AASSET_MODE_STREAMING);

    if (asset) {
        AAsset_close(asset);
        return true;
    }
    LogE("%s exists: filePath=%s AAssetManager_open is error.", TAG, filePath);
    return false;
}

size_t AssetManagerReader::getFileSize(const char* filePath) {
    std::lock_guard<std::mutex> lock(mMutex);
    if (!mAssetManager || !filePath) {
        return 0;
    }

    AAssetManager* mgr = static_cast<AAssetManager*>(mAssetManager);
    AAsset* asset = AAssetManager_open(mgr, filePath, AASSET_MODE_STREAMING);

    size_t size = 0;
    if (asset) {
        size = static_cast<size_t>(AAsset_getLength(asset));
        AAsset_close(asset);
    }
    return size;
}
