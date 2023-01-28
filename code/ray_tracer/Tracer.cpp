#include "Tracer.hpp"

#include <glm/gtx/string_cast.hpp>
#include <stdexcept>
#include <algorithm>
#include <random>

#include "gloo/Transform.hpp"
#include "gloo/components/MaterialComponent.hpp"
#include "gloo/lights/AmbientLight.hpp"

#include "gloo/Image.hpp"
#include "Illuminator.hpp"

#include "glm/gtx/string_cast.hpp"

namespace GLOO {
void Tracer::Render(const Scene& scene, const std::string& output_file) {
  scene_ptr_ = &scene;

  auto& root = scene_ptr_->GetRootNode();
  tracing_components_ = root.GetComponentPtrsInChildren<TracingComponent>();
  light_components_ = root.GetComponentPtrsInChildren<LightComponent>();

  Image image(image_size_.x, image_size_.y);
  if (filter_enabled_ || jitter_enabled_) {
    Image super_image(image_size_.x * filter_grid_size_, image_size_.y * filter_grid_size_);
    MakeImage(super_image, jitter_enabled_);

    for (size_t y = 0; y < image_size_.y; y++) {
      size_t super_y = filter_grid_size_ * y;
      for (size_t x = 0; x < image_size_.x; x++) {
        // For each pixel, cast a ray to the appropriately scaled spot in scene
        size_t super_x = filter_grid_size_ * x;
        glm::vec3 color = glm::vec3(0.f);
        for (size_t i = 0; i < filter_grid_size_; i++) {
          for (size_t j = 0; j < filter_grid_size_; j++) {
            color += super_image.GetPixel(super_x + i, super_y + j);
          }
        }
        // Update its value in the image.
        float grid_size = filter_grid_size_ * filter_grid_size_;
        image.SetPixel(x, y, color / grid_size);
      }
    }
  }
  else {
    MakeImage(image);
  }

  if (output_file.size())
    image.SavePNG(output_file);
}

void Tracer::MakeImage(Image& original_image, bool jittering) {
  size_t image_x = original_image.GetWidth();
  size_t image_y = original_image.GetHeight();
  float scaled_size_x = 2.0f / image_x;
  float scaled_size_y = 2.0f / image_y;
  for (size_t y = 0; y < image_y; y++) {
      for (size_t x = 0; x < image_x; x++) {
        // For each pixel, cast a ray to the appropriately scaled spot in scene
        float x_scaled = (scaled_size_x * x) - 1;
        float y_scaled = (scaled_size_y * y) - 1;
        if (jittering) {
          x_scaled += GetRandomFloat(scaled_size_x / 2, -scaled_size_x / 2);
          y_scaled += GetRandomFloat(scaled_size_y / 2, -scaled_size_y / 2);
        }
        HitRecord record;
        glm::vec3 color;
        if (camera_type_ == CameraType::Perspective)
          color = TraceRay(perspective_camera_.GenerateRay(glm::vec2(x_scaled, y_scaled)), max_bounces_, record);
        else if (camera_type_ == CameraType::Orthographic)
          color = TraceRay(orthographic_camera_.GenerateRay(glm::vec2(x_scaled, y_scaled), camera_size_), max_bounces_, record);
        else if (camera_type_ == CameraType::Multiperspective)
          color = TraceRay(multiperspective_camera_.GenerateRay(glm::vec2(x_scaled, y_scaled), camera_size_), max_bounces_, record);
        
        // Update its value in the image.
        original_image.SetPixel(x, y, color);
      }
    }
}

float Tracer::GetRandomFloat(float a, float b)
{
    static std::default_random_engine e;
    static std::uniform_real_distribution<>dis(a, b); // rage 0 - 1
    return dis(e);
}

glm::vec3 Tracer::TraceRay(const Ray& ray,
                           size_t bounces,
                           HitRecord& record) const {
  // Finding the closest tracing component with the cast ray.
  TracingComponent * closest_tracing_component = nullptr;

  for (TracingComponent * const tracing_component : tracing_components_) {
    glm::mat4 tranformation_matrix = tracing_component->GetNodePtr()->GetTransform().GetLocalToWorldMatrix();
    Ray new_ray = Ray(ray.GetOrigin(), ray.GetDirection());
    new_ray.ApplyTransform(glm::inverse(tranformation_matrix));
    if (tracing_component->GetHittable().Intersect(new_ray, 0.f, record)) {
      glm::vec4 overloaded_normal = glm::transpose(glm::inverse(tranformation_matrix)) * glm::vec4(record.normal, 1.f);
      record.normal = glm::normalize(glm::vec3(overloaded_normal));
      closest_tracing_component = tracing_component;
    }
  }

  if (closest_tracing_component == nullptr) {
    return GetBackgroundColor(ray.GetDirection());
  }

  // Where the ray hit the object
  const glm::vec3 hit_pos = ray.At(record.time);

  // Use the Phong shading model

  // The material of the closest object intersected
  MaterialComponent * closest_material_component = closest_tracing_component->GetNodePtr()->GetComponentPtr<MaterialComponent>();
  glm::vec3 k_diffuse = closest_material_component->GetMaterial().GetDiffuseColor(); // The diffuse coefficient of the material
  glm::vec3 k_specular = closest_material_component->GetMaterial().GetSpecularColor(); // The specular coefficient of the material
  float shininess = closest_material_component->GetMaterial().GetShininess(); // The shininess of the material

  // The reflection direction
  glm::vec3 perfect_reflection = glm::normalize(glm::reflect(ray.GetDirection(), record.normal));

  // Get the ambient illumination
  glm::vec3 illumination_ambient;
  for (LightComponent * light_component : light_components_) {
    if (light_component->GetLightPtr()->GetType() == LightType::Ambient) {
      illumination_ambient = light_component->GetLightPtr()->GetDiffuseColor();
    }
  }

  glm::vec3 final_intensity = illumination_ambient * k_diffuse;
  float e = 1e-3;

  for (LightComponent * light_component : light_components_) {
    if (light_component->GetLightPtr()->GetType() == LightType::Ambient) {
      continue;
    }

    glm::vec3 dir_to_light, intensity;
    float dist_to_light;
    Illuminator::GetIllumination(*light_component, hit_pos, dir_to_light, intensity, dist_to_light);
    HitRecord shadow_record;

    // Casting the shadow rays
    if (shadows_enabled_) {
      for (TracingComponent * const other_component : tracing_components_) {
        Ray shadow_ray = Ray(hit_pos, dir_to_light);
        glm::mat4 other_tranformation_matrix = other_component->GetNodePtr()->GetTransform().GetLocalToWorldMatrix();
        shadow_ray.ApplyTransform(glm::inverse(other_tranformation_matrix));
        other_component->GetHittable().Intersect(shadow_ray, e, shadow_record);
      }
      float dist = shadow_record.time;
      if (dist < dist_to_light) {
        continue;
      }
    }

    // Finding the diffuse shading for this pixel
    float clamp_diffuse = glm::dot(dir_to_light, record.normal);
    clamp_diffuse = clamp_diffuse > 0.f ? clamp_diffuse : 0.f;
    glm::vec3 illumination_diffuse = clamp_diffuse * intensity * k_diffuse;

    // Finding the specular shading for this pixel
    float clamp_specular = glm::dot(dir_to_light, perfect_reflection);
    clamp_specular = clamp_specular > 0.f ? clamp_specular : 0.f;
    glm::vec3 illumination_specular = pow(clamp_specular, shininess) * intensity * k_specular;

    final_intensity += illumination_diffuse + illumination_specular;
  }

  glm::vec3 illumination_indirect = glm::vec3(0.f);
  // Send another ray for indirect illumination (reflections)
  if (bounces > 0) {
    Ray another_ray = Ray(hit_pos, perfect_reflection);
    Ray moved_ray = Ray(another_ray.At(e), perfect_reflection);
    HitRecord new_record;
    illumination_indirect = TraceRay(moved_ray, bounces - 1, new_record);
  }
  
  return final_intensity + (k_specular * illumination_indirect);
}


glm::vec3 Tracer::GetBackgroundColor(const glm::vec3& direction) const {
  if (cube_map_ != nullptr) {
    return cube_map_->GetTexel(direction);
  } else
    return background_color_;
}
}  // namespace GLOO
