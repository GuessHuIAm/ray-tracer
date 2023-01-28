#include "Plane.hpp"

#include <cmath>

#include <glm/gtx/norm.hpp>

namespace GLOO {
bool Plane::Intersect(const Ray& ray, float t_min, HitRecord& record) const {
  // Implement ray-plane intersection.

  // The point of intersection
  if (glm::dot(normal_, ray.GetDirection()) == 0.f) {
    return false;
  }
  float t = (offset_ - glm::dot(normal_, ray.GetOrigin())) / glm::dot(normal_, ray.GetDirection());
  if (t < t_min)
      return false;
  if (t < record.time) {
      record.time = t;
      record.normal = normal_;
      return true;
  }
  return false;
}
}  // namespace GLOO
