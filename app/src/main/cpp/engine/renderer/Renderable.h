#pragma once
#include <glm/glm.hpp>

namespace engine {
namespace renderer {

class Mesh;
class Material;

struct Renderable {
    Mesh& mesh;
    Material& material;
    glm::mat4 worldMatrix;

    Renderable(Mesh& m, Material& mat, const glm::mat4& world)
        : mesh(m), material(mat), worldMatrix(world) {}
};

} // namespace renderer
} // namespace engine
