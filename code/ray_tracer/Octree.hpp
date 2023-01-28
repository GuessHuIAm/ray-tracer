#ifndef OCTREE_H_
#define OCTREE_H_

#include <memory>

#include <glm/glm.hpp>

#include "HitRecord.hpp"
#include "hittable/Triangle.hpp"

namespace GLOO {
// Forward declarations.
class Mesh;

struct AABB {
  AABB() {
  }
  AABB(const glm::vec3& _mn, const glm::vec3& _mx) : mn(_mn), mx(_mx) {
  }
  AABB(float mnx, float mny, float mnz, float mxx, float mxy, float mxz)
      : mn(glm::vec3(mnx, mny, mnz)), mx(glm::vec3(mxx, mxy, mxz)) {
  }
  static AABB FromTriangle(const Triangle& triangle);
  static AABB FromMesh(const Mesh& mesh);

  void UnionWith(const AABB& other);
  bool Overlap(const AABB& other) const;
  bool Contain(const AABB& other) const;

  glm::vec3 mn, mx;
};

class Octree {
 public:
  Octree(int max_level = 8) : max_level_(max_level) {
  }
  void Build(const Mesh& mesh);
  bool Intersect(const Ray& ray, float t_min, HitRecord& record);

 private:
  struct OctNode {
    bool IsTerminal() const {
      return child[0] == nullptr;
    }

    std::unique_ptr<OctNode> child[8];
    std::vector<const Triangle*> triangles;
  };

  void BuildNode(OctNode& node,
                 const AABB& bbox,
                 const std::vector<const Triangle*>& triangles,
                 int level);

  bool IntersectSubtree(uint8_t aa,
                        const OctNode& node,
                        float tx0,
                        float ty0,
                        float tz0,
                        float tx1,
                        float ty1,
                        float tz1,
                        const Ray& r,
                        float t_min,
                        HitRecord& record);

  int max_level_;
  AABB bbox_;
  std::unique_ptr<OctNode> root_;
};
}  // namespace GLOO

#endif
