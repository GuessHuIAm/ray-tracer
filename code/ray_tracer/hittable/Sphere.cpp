#include "Sphere.hpp"

#include <cmath>

#include <glm/gtx/norm.hpp>

namespace GLOO {
bool Sphere::Intersect(const Ray& ray, float t_min, HitRecord& record) const {
  float a = glm::length2(ray.GetDirection());
  float b = 2 * glm::dot(ray.GetDirection(), ray.GetOrigin());
  float c = glm::length2(ray.GetOrigin()) - radius_ * radius_;

  float d = b * b - 4 * a * c;

  if (d < 0) {
    return false;
  }
  d = sqrt(d);

  float t_plus = (-b + d) / (2 * a);
  float t_minus = (-b - d) / (2 * a);

  float t;
  if (t_minus < t_min) {
    if (t_plus < t_min)
      return false;
    else {
      t = t_plus;
    }
  } else {
    t = t_minus;
  }

  if (t < record.time) {
    record.time = t;
    record.normal = glm::normalize(ray.At(t));
    return true;
  }

  return false;
}
}  // namespace GLOO
