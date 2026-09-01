#pragma once

namespace engine {
class Transform;
namespace renderer {

class Mesh;
class Material;

struct Renderable {
    Mesh& mesh;
    Material& material;
    Transform& transform;

    Renderable(Mesh& m, Material& mat, Transform& t)
        : mesh(m), material(mat), transform(t) {}
};

} // namespace renderer
} // namespace engine
