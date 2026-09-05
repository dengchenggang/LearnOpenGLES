#include "MeshRenderer.h"
#include "Actor.h"
#include "Camera.h"
#include "Mesh.h"
#include "Material.h"
#include "Transform.h"

namespace engine {

MeshRenderer::MeshRenderer(Actor& owner) : SceneComponent(owner) {}

renderer::Renderable MeshRenderer::getRenderable() const {
    return renderer::Renderable(*mMesh, *mMaterial, getTransform());
}

void MeshRenderer::onRender() {
    if (!mMesh || !mMaterial || !mCamera) {
        return;
    }

    glm::mat4 mvp = mCamera->getProjectionMatrix() *
                    mCamera->getViewMatrix() *
                    getTransform().getModelMatrix();

    mMaterial->setUniformMat4("uMVP", mvp);
    mMaterial->bind();
    mMesh->draw();
    mMaterial->unbind();
}

} // namespace engine
