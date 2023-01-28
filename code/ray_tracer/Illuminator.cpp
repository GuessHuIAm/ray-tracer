#include "Illuminator.hpp"

#include <limits>
#include <stdexcept>

#include <glm/geometric.hpp>

#include "gloo/lights/DirectionalLight.hpp"
#include "gloo/lights/PointLight.hpp"
#include "gloo/SceneNode.hpp"

#include "glm/gtx/string_cast.hpp"

namespace GLOO {
void Illuminator::GetIllumination(const LightComponent& light_component,
                                  const glm::vec3& hit_pos,
                                  glm::vec3& dir_to_light,
                                  glm::vec3& intensity,
                                  float& dist_to_light) {
  // Calculation will be done in world space.

  auto light_ptr = light_component.GetLightPtr();
  if (light_ptr->GetType() == LightType::Directional) {
    auto directional_light_ptr = static_cast<DirectionalLight*>(light_ptr);
    dir_to_light = -directional_light_ptr->GetDirection();
    intensity = directional_light_ptr->GetDiffuseColor();
    dist_to_light = std::numeric_limits<float>::max();
  } else if (light_ptr->GetType() == LightType::Point) {
    // Implement point light.
    auto point_light_ptr = static_cast<PointLight*>(light_ptr);
    glm::vec3 light_source_position = light_component.GetNodePtr()->GetTransform().GetPosition();
    dir_to_light = glm::normalize(light_source_position - hit_pos);
    dist_to_light = glm::distance(light_source_position, hit_pos);
    intensity = point_light_ptr->GetDiffuseColor() / (point_light_ptr->GetAttenuation().x * dist_to_light * dist_to_light);
  } else {
      throw std::runtime_error (
      "Unrecognized light type when computing "
      "illumination" );
  }
}
}  // namespace GLOO
