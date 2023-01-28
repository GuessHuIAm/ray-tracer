#ifndef PERSPECTIVE_CAMERA_H_
#define PERSPECTIVE_CAMERA_H_

#include <cmath>
#include <glm/ext/quaternion_geometric.hpp>

#include "gloo/utils.hpp"

#include "Ray.hpp"
#include "CameraSpec.hpp"

namespace GLOO {
class PerspectiveCamera {
 public:
  PerspectiveCamera(const CameraSpec& spec) {
    center_ = spec.center;
    direction_ = glm::normalize(spec.direction);
    up_ = glm::normalize(spec.up);
    fov_radian_ = ToRadian(spec.fov);
    horizontal_ = glm::normalize(glm::cross(direction_, up_));
  }

  Ray GenerateRay(const glm::vec2& point, float size=1.f) {
    float d = 1.0f / tanf(fov_radian_ / 2.0f);
    glm::vec3 new_dir =
        d * direction_ + point[0] * horizontal_ + point[1] * up_;
    new_dir = glm::normalize(new_dir);

    return Ray(center_, new_dir);
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
