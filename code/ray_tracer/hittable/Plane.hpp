#ifndef PLANE_H_
#define PLANE_H_

#include "HittableBase.hpp"

namespace GLOO {
class Plane : public HittableBase {
 public:
  Plane(const glm::vec3& normal, float d) : normal_(normal), offset_(d) {
  }
  bool Intersect(const Ray& ray, float t_min, HitRecord& record) const override;

 private:
  glm::vec3 normal_;
  float offset_;
};
}  // namespace GLOO

#endif
