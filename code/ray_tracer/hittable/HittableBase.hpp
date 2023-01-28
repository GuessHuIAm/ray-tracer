#ifndef HITTABLE_BASE_H_
#define HITTABLE_BASE_H_

#include "Ray.hpp"
#include "HitRecord.hpp"

namespace GLOO {
class HittableBase {
 public:
  // It is assumed that ray is in the local coordinates.
  virtual bool Intersect(const Ray& ray,
                         float t_min,
                         HitRecord& record) const = 0;
  virtual ~HittableBase() {
  }
};
}  // namespace GLOO

#endif
