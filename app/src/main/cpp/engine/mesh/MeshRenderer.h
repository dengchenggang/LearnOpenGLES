#pragma once

#include "SceneComponent.h"
#include "Renderable.h"
#include <memory>

namespace engine {

class Camera;

class MeshRenderer : public SceneComponent {
public:
    MeshRenderer(Actor& owner);
    ~MeshRenderer() override = default;

    MeshRenderer(const MeshRenderer&) = delete;
    MeshRenderer& operator=(const MeshRenderer&) = delete;
public:
    void setMesh(std::shared_ptr<renderer::Mesh> mesh) { mMesh = std::move(mesh); }
    void setMaterial(std::shared_ptr<renderer::Material> material) { mMaterial = std::move(material); }

    std::shared_ptr<renderer::Mesh> getMesh() const { return mMesh; }
    std::shared_ptr<renderer::Material> getMaterial() const { return mMaterial; }

    renderer::Renderable getRenderable() const;

protected:
    void onBeginPlay() override {}
    void onEnabledChanged(bool enabled) override {}
    void onUpdate(float deltaTime) override {}
    void onRender() override;
    void onEndPlay() override {}
    void onAttachedTo(SceneComponent* parent) override { SceneComponent::onAttachedTo(parent); }
    void onVisibilityChanged(bool visible) override { SceneComponent::onVisibilityChanged(visible); }
    void onDetachedFrom(SceneComponent* parent) override { SceneComponent::onDetachedFrom(parent); }

private:
    std::shared_ptr<renderer::Mesh> mMesh;
    std::shared_ptr<renderer::Material> mMaterial;

    // MVP 矩阵缓存
    glm::mat4 mCachedMVP = glm::mat4(0.0f);
    uint64_t mLastModelVersion = 0;
    uint64_t mLastViewVersion = 0;
    uint64_t mLastProjectionVersion = 0;
};

} // namespace engine
