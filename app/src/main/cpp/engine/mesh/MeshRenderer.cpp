#include "MeshRenderer.h"
#include "Actor.h"
#include "Camera.h"
#include "Mesh.h"
#include "Material.h"
#include "Transform.h"

namespace engine {

MeshRenderer::MeshRenderer(Actor& owner) : SceneComponent(owner) {}

renderer::Renderable MeshRenderer::getRenderable() const {
    return renderer::Renderable(*mMesh, *mMaterial, getWorldMatrix());
}

void MeshRenderer::onRender() {
    if (!mMesh || !mMaterial) {
        return;
    }

    auto& camera = GetOwner().getCamera();
    glm::mat4 mvp = camera.getProjectionMatrix() *
                    camera.getViewMatrix() *
                    getWorldMatrix();

    mMaterial->bind();
    mMaterial->setUniformMat4("uMVP", mvp);
    mMesh->draw();
    mMaterial->unbind();
}

} // namespace engine
