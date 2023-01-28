#ifndef SPHERE_H_
#define SPHERE_H_

#include "HittableBase.hpp"

namespace GLOO {
class Sphere : public HittableBase {
 public:
  // A sphere is always centered at origin in its local coordinate.
  Sphere(float radius) : radius_(radius) {
  }
  bool Intersect(const Ray& ray, float t_min, HitRecord& record) const override;

 private:
  float radius_;
};
}  // namespace GLOO

#endif
