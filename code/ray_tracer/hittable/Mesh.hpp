#ifndef MESH_H_
#define MESH_H_

#include "HittableBase.hpp"

#include "gloo/alias_types.hpp"

#include "Triangle.hpp"
#include "Octree.hpp"

namespace GLOO {
class Octree;

class Mesh : public HittableBase {
 public:
  Mesh(std::unique_ptr<PositionArray> positions,
       std::unique_ptr<NormalArray> normals,
       std::unique_ptr<IndexArray> indices);

  bool Intersect(const Ray& ray, float t_min, HitRecord& record) const override;
  const std::vector<Triangle>& GetTriangles() const {
    return triangles_;
  }

 private:
  std::vector<Triangle> triangles_;
  std::unique_ptr<Octree> octree_;
};
}  // namespace GLOO

#endif
