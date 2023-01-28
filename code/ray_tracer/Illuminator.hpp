#ifndef ILLUMINATOR_H_
#define ILLUMINATOR_H_

#include "gloo/lights/LightBase.hpp"
#include "gloo/components/LightComponent.hpp"

namespace GLOO {
class Illuminator {
 public:
  static void GetIllumination(const LightComponent& light_component,
                              const glm::vec3& world_pos,
                              glm::vec3& dir_to_light,
                              glm::vec3& intensity,
                              float& dist_to_light);
};
}  // namespace GLOO

#endif
