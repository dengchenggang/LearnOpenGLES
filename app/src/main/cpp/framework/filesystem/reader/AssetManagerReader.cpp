#include "AssetManagerReader.h"
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include "Log.h"

constexpr const char* TAG {"AssetManagerReader"};

void AssetManagerReader::initialize(void* context) {
    std::lock_guard<std::mutex> lock(mMutex);
    if (!mAssetManager) {
        mAssetManager = context;
    }
}

bool AssetManagerReader::isInitialized() const {
    std::lock_guard<std::mutex> lock(mMutex);
    return mAssetManager != nullptr;
}

FileData AssetManagerReader::readFile(const char* filePath) {
    FileData result;

    std::lock_guard<std::mutex> lock(mMutex);
    if (!mAssetManager || !filePath) {
        return result;
    }

    AAssetManager* mgr = static_cast<AAssetManager*>(mAssetManager);
    AAsset* asset = AAssetManager_open(mgr, filePath, AASSET_MODE_STREAMING);

    if (!asset) {
        return result;
    }

    off_t fileSize = AAsset_getLength(asset);
    if (fileSize > 0) {
        result.data.resize(static_cast<size_t>(fileSize));
        off_t bytesRead = AAsset_read(asset, result.data.data(), static_cast<size_t>(fileSize));
        if (bytesRead != fileSize) {
            result.data.clear();
        }
    }

    AAsset_close(asset);
    return result;
}

std::string AssetManagerReader::readString(const char* filePath) {
    FileData data = readFile(filePath);
    return data.asString();
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
