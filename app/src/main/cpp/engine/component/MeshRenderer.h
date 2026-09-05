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

    void setMesh(std::shared_ptr<renderer::Mesh> mesh) { mMesh = std::move(mesh); }
    void setMaterial(std::shared_ptr<renderer::Material> material) { mMaterial = std::move(material); }
    void setCamera(const std::shared_ptr<Camera>& camera) { mCamera = camera; }

    std::shared_ptr<renderer::Mesh> getMesh() const { return mMesh; }
    std::shared_ptr<renderer::Material> getMaterial() const { return mMaterial; }

    renderer::Renderable getRenderable() const;

    void onRender() override;

private:
    std::shared_ptr<renderer::Mesh> mMesh;
    std::shared_ptr<renderer::Material> mMaterial;
    std::shared_ptr<Camera> mCamera;
};

} // namespace engine
