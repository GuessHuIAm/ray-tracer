#ifndef ORTHOGRAPHIC_CAMERA_H_
#define ORTHOGRAPHIC_CAMERA_H_

#include <cmath>
#include <glm/ext/quaternion_geometric.hpp>

#include "gloo/utils.hpp"

#include "Ray.hpp"
#include "CameraSpec.hpp"

namespace GLOO {
class OrthographicCamera {
 public:
  OrthographicCamera(const CameraSpec& spec) {
    center_ = spec.center;
    direction_ = glm::normalize(spec.direction);
    up_ = glm::normalize(spec.up);
    fov_radian_ = ToRadian(spec.fov);
    horizontal_ = glm::normalize(glm::cross(direction_, up_));
  }

  Ray GenerateRay(const glm::vec2& point, float size=1.f) {
    glm::vec3 origin = center_ + (point.x * size * horizontal_) + (point.y * size * up_);
    return Ray(origin, direction_);
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
