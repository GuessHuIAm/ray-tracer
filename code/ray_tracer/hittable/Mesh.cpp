#include "Mesh.hpp"

#include <functional>
#include <stdexcept>
#include <iostream>

#include "gloo/utils.hpp"

namespace GLOO {
Mesh::Mesh(std::unique_ptr<PositionArray> positions,
           std::unique_ptr<NormalArray> normals,
           std::unique_ptr<IndexArray> indices) {
  size_t num_vertices = indices->size();
  if (num_vertices % 3 != 0 || normals->size() != positions->size())
    throw std::runtime_error("Bad mesh data in Mesh constuctor!");

  for (size_t i = 0; i < num_vertices; i += 3) {
    triangles_.emplace_back(
        positions->at(indices->at(i)), positions->at(indices->at(i + 1)),
        positions->at(indices->at(i + 2)), normals->at(indices->at(i)),
        normals->at(indices->at(i + 1)), normals->at(indices->at(i + 2)));
  }
  // Let mesh data destruct.

  // Build Octree.
  octree_ = make_unique<Octree>();
  octree_->Build(*this);
}

bool Mesh::Intersect(const Ray& ray, float t_min, HitRecord& record) const {
  return octree_->Intersect(ray, t_min, record);
}
}  // namespace GLOO
