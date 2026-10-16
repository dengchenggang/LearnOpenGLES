#include "MeshRenderer.h"
#include "Actor.h"
#include "Camera.h"
#include "Mesh.h"
#include "Material.h"
#include "Transform.h"

namespace engine {

MeshRenderer::MeshRenderer(Actor& owner) : SceneComponent(owner) {}

renderer::Renderable MeshRenderer::getRenderable() const {
    return renderer::Renderable(*mMesh, *mMaterial, getWorldMatrix().first);
}

void MeshRenderer::onRender() {
    SceneComponent::onRender();
    if (!mMesh || !mMaterial) {
        return;
    }

    mMaterial->bind();

    auto& camera = GetOwner().getCamera();
    auto& modelPair = getWorldMatrix();
    auto& projectionPair = camera.getProjectionMatrix();
    auto& viewPair = camera.getViewMatrix();

    if (mLastModelVersion != modelPair.second ||
        mLastProjectionVersion != projectionPair.second ||
        mLastViewVersion != viewPair.second) {
        mCachedMVP = projectionPair.first * viewPair.first * modelPair.first;
        mMaterial->setUniformMat4("uMVP", mCachedMVP);
        mLastModelVersion = modelPair.second;
        mLastProjectionVersion = projectionPair.second;
        mLastViewVersion = viewPair.second;
    }

    mMesh->draw();
    mMaterial->unbind();
}

} // namespace engine
