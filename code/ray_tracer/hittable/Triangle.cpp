#include "Triangle.hpp"

#include <iostream>
#include <stdexcept>

#include <glm/common.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Plane.hpp"

namespace GLOO {
Triangle::Triangle(const glm::vec3& p0,
                   const glm::vec3& p1,
                   const glm::vec3& p2,
                   const glm::vec3& n0,
                   const glm::vec3& n1,
                   const glm::vec3& n2) {
    positions_.push_back(p0);
    positions_.push_back(p1);
    positions_.push_back(p2);
    normals_.push_back(n0);
    normals_.push_back(n1);
    normals_.push_back(n2);
    a = GetPosition(0);
    b = GetPosition(1);
    c = GetPosition(2);
}

bool Triangle::Intersect(const Ray& ray, float t_min, HitRecord& record) const {
  // Implement ray-triangle intersection.

  glm::vec3 origin = ray.GetOrigin();
  glm::vec3 direction = ray.GetDirection();

  glm::mat3 A_matrix = glm::mat3(a - b, a - c, direction);
  glm::vec3 b_vector = a - origin;
  glm::vec3 x_vector = glm::inverse(A_matrix) * b_vector;
  float beta = x_vector[0];
  float gamma = x_vector[1];
  float t = x_vector[2];

  if (beta + gamma <= 1.f &&
      beta >= 0 &&
      gamma >= 0 &&
      t > t_min &&
      t < record.time) {
    float alpha = 1.f - beta - gamma;
    record.time = t;
    // record.normal = glm::normalize(glm::cross(GetPosition(0) - GetPosition(1), GetPosition(0) - GetPosition(2)));
    record.normal = alpha * GetNormal(0) + beta * GetNormal(1) + gamma * GetNormal(2); // Same as the above approach
    return true;
  }

  return false;
}
}  // namespace GLOO
