#ifndef MULTIPERSPECTIVE_CAMERA_H_
#define MULTIPERSPECTIVE_CAMERA_H_

#include <cmath>
#include <glm/ext/quaternion_geometric.hpp>

#include "gloo/utils.hpp"

#include "Ray.hpp"
#include "CameraSpec.hpp"

namespace GLOO {
class MultiperspectiveCamera {
 public:
  MultiperspectiveCamera(const CameraSpec& spec) {
    center_ = spec.center;
    direction_ = glm::normalize(spec.direction);
    up_ = glm::normalize(spec.up);
    fov_radian_ = ToRadian(spec.fov);
    horizontal_ = glm::normalize(glm::cross(direction_, up_));
  }

  Ray GenerateRay(const glm::vec2& point, float size=1.f) {
    glm::vec3 dummy_center = center_ + (direction_ * size);
    glm::vec3 new_center = center_ + (point.x * horizontal_ * size) + ((point.x * point.x * direction_ * size));
    glm::vec3 new_direction = glm::normalize(dummy_center - new_center);
    glm::vec3 new_horizontal = glm::normalize(glm::cross(new_direction, up_));

    float d = 1.0f / tanf(fov_radian_ / 2.0f);
    glm::vec3 new_dir = glm::normalize(d * new_direction + point[0] * new_horizontal + point[1] * up_);

    return Ray(new_center, new_dir);
  }

  float GetTMin() const {
    return 0.0f;
  }

 private:
  glm::vec3 center_;
  glm::vec3 direction_;
  glm::vec3 up_;
  float fov_radian_;
  glm::vec3 horizontal_;
};
}  // namespace GLOO

#endif
