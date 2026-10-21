#pragma once
#include <string>
#include <cstdint>
#include <map>
#include <glm/glm.hpp>
#include "Level.h"
namespace engine {
class Camera;

class LevelManager {
public:
    void changeLevel(const std::string& levelName);

    void applyLevelViewport(const glm::vec4& logicalRect, const glm::vec4& renderRect) {
        onLogicalViewportChanged(logicalRect);
        onRenderViewportChanged(renderRect);
    }

    void setViewportSize(int32_t width, int32_t height) {
        mViewportWidth = width;
        mViewportHeight = height;
    }

    int32_t getViewportWidth() const { return mViewportWidth; }
    int32_t getViewportHeight() const { return mViewportHeight; }

protected:
    virtual void onLogicalViewportChanged(const glm::vec4& rect) = 0;
    virtual void onRenderViewportChanged(const glm::vec4& rect) = 0;

    void initLevels(std::map<std::string, LevelPtr>&& levels, Camera& camera);
    void updateLevels(int64_t deltaTime);
    void doChangeLevel(const std::string& levelName);
    void renderLevels();
    void deInitLevels();
    void resizeLevel(int32_t width, int32_t height);

protected:
    int32_t mViewportWidth = 0;
    int32_t mViewportHeight = 0;
    std::map<std::string, LevelPtr> mLevels;
    Level* mActiveLevel = nullptr;
    std::string mNextActiveLevelName;
    std::string mPreviousActiveLevelName;
};
}