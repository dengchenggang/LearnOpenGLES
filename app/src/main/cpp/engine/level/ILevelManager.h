#pragma once
#include <string>
#include <cstdint>

namespace engine {
class ILevelManager {
public:
    virtual void changeLevel(const std::string& levelName) = 0;

    void setViewportSize(int32_t width, int32_t height) {
        mViewportWidth = width;
        mViewportHeight = height;
    }

    int32_t getViewportWidth() const { return mViewportWidth; }
    int32_t getViewportHeight() const { return mViewportHeight; }

protected:
    int32_t mViewportWidth = 0;
    int32_t mViewportHeight = 0;
};
}