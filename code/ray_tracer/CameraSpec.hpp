#ifndef CAMERA_SPEC_H_
#define CAMERA_SPEC_H_

#include <glm/glm.hpp>

namespace GLOO {
struct CameraSpec {
  glm::vec3 center;
  glm::vec3 direction;
  glm::vec3 up;
  float fov;
};
}  // namespace GLOO

#endif
