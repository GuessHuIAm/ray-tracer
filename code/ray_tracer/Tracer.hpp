#ifndef TRACER_H_
#define TRACER_H_

#include "gloo/Scene.hpp"
#include "gloo/Material.hpp"
#include "gloo/lights/LightBase.hpp"
#include "gloo/components/LightComponent.hpp"

#include "Ray.hpp"
#include "HitRecord.hpp"
#include "TracingComponent.hpp"
#include "CubeMap.hpp"
#include "CameraType.hpp"
#include "PerspectiveCamera.hpp"
#include "OrthographicCamera.hpp"
#include "MultiperspectiveCamera.hpp"

namespace GLOO {
class Tracer {
 public:
  Tracer(const CameraSpec& camera_spec,
         const CameraType camera_type,
         float camera_size,
         const glm::ivec2& image_size,
         size_t max_bounces,
         const glm::vec3& background_color,
         const CubeMap* cube_map,
         bool shadows_enabled,
         bool filter_enabled,
         size_t filter_grid_size,
         bool jitter_enabled)
      : perspective_camera_(camera_spec),
        orthographic_camera_(camera_spec),
        multiperspective_camera_(camera_spec),
        camera_type_(camera_type),
        camera_size_(camera_size),
        image_size_(image_size),
        max_bounces_(max_bounces),
        background_color_(background_color),
        cube_map_(cube_map),
        shadows_enabled_(shadows_enabled),
        filter_enabled_(filter_enabled),
        filter_grid_size_(filter_grid_size),
        jitter_enabled_(jitter_enabled),
        scene_ptr_(nullptr) {
  }
  void Render(const Scene& scene, const std::string& output_file);

 private:
  void MakeImage(Image& original_image, bool jittering=false);
  float GetRandomFloat(float a, float b);
  glm::vec3 TraceRay(const Ray& ray, size_t bounces, HitRecord& record) const;

  glm::vec3 GetBackgroundColor(const glm::vec3& direction) const;

  PerspectiveCamera perspective_camera_;
  OrthographicCamera orthographic_camera_;
  MultiperspectiveCamera multiperspective_camera_;
  CameraType camera_type_;
  float camera_size_;
  glm::ivec2 image_size_;
  size_t max_bounces_;

  std::vector<TracingComponent*> tracing_components_;
  std::vector<LightComponent*> light_components_;
  glm::vec3 background_color_;
  const CubeMap* cube_map_;
  bool shadows_enabled_;
  bool filter_enabled_;
  size_t filter_grid_size_;
  bool jitter_enabled_;

  const Scene* scene_ptr_;
};
}  // namespace GLOO

#endif
