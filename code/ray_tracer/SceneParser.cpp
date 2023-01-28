#include "SceneParser.hpp"

#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/string_cast.hpp>
#include <sstream>
#include <stdexcept>

#include "gloo/utils.hpp"
#include "gloo/components/MaterialComponent.hpp"
#include "gloo/components/LightComponent.hpp"
#include "gloo/lights/PointLight.hpp"
#include "gloo/lights/DirectionalLight.hpp"
#include "gloo/lights/AmbientLight.hpp"
#include "gloo/parsers/ObjParser.hpp"

#include "helpers.hpp"

#include "TracingComponent.hpp"
#include "hittable/Sphere.hpp"
#include "hittable/Plane.hpp"
#include "hittable/Triangle.hpp"
#include "hittable/Mesh.hpp"

namespace GLOO {
SceneParser::SceneParser() {
}

std::unique_ptr<Scene> SceneParser::ParseScene(const std::string& filename) {
  std::string file_path = GetAssetDir() + filename;
  fs_ = std::fstream(file_path);
  if (!fs_) {
    std::cerr << "ERROR: Unable to open scene file " + file_path + "!"
              << std::endl;
    return nullptr;
  }

  base_path_ = GetBasePath(file_path);

  std::unique_ptr<Scene> scene;
  std::string token;
  while (fs_ >> token) {
    // In each Parse* the enclosing brackets will be read.
    if (token == "Background") {
      ParseBackground();
    } else if (token == "Camera") {
      ParseCamera();
    } else if (token == "Materials") {
      ParseMaterials();
    } else if (token == "Scene") {
      scene = make_unique<Scene>(ParseSceneNode());
    } else if (token != "}") {
      throw std::runtime_error("Bad scene token: " + token + "!");
    }
  }

  // Add in ambient light.
  auto ambient_light = std::make_shared<AmbientLight>();
  ambient_light->SetAmbientColor(background_.ambient_light);
  auto ambient_light_node = make_unique<SceneNode>();
  ambient_light_node->CreateComponent<LightComponent>(std::move(ambient_light));
  scene->GetRootNode().AddChild(std::move(ambient_light_node));

  return scene;
}

void SceneParser::Assert(const std::string& token,
                         const std::string& expected) {
  if (token != expected) {
    throw std::runtime_error("Token " + token + " does not match expected " +
                             expected + "!");
  }
}

void SceneParser::ParseBackground() {
  std::string token;
  fs_ >> token;
  Assert(token, "{");
  while (token != "}") {
    fs_ >> token;
    if (token == "color") {
      background_.color = ReadVec3();
    } else if (token == "ambient_light") {
      background_.ambient_light = ReadVec3();
    } else if (token == "cube_map") {
      std::string dir;
      fs_ >> dir;
      background_.cube_map = make_unique<CubeMap>(base_path_ + dir);
    } else if (token != "}") {
      throw std::runtime_error("Bad background token: " + token + "!");
    }
  }
}

void SceneParser::ParseMaterials() {
  materials_.clear();
  std::string token;
  fs_ >> token;
  Assert(token, "{");
  while (true) {
    fs_ >> token;
    if (token == "}")
      break;
    Assert(token, "Material");
    materials_.push_back(ParseMaterial());
  }
}

std::shared_ptr<Material> SceneParser::ParseMaterial() {
  auto material = std::make_shared<Material>();
  material->SetDiffuseColor(glm::vec3(1.0f));
  material->SetSpecularColor(glm::vec3(0.0f));
  material->SetShininess(0.0f);

  std::string token;
  fs_ >> token;
  Assert(token, "{");
  while (token != "}") {
    fs_ >> token;
    if (token == "diffuse") {
      glm::vec3 color = ReadVec3();
      // Treat ambient and diffuse colors the same.
      material->SetAmbientColor(color);
      material->SetDiffuseColor(color);
    } else if (token == "specular") {
      glm::vec3 color = ReadVec3();
      material->SetSpecularColor(color);
    } else if (token == "shininess") {
      material->SetShininess(ReadFloat());
    } else if (token != "}") {
      throw std::runtime_error("Bad material token " + token + "!");
    }
  }

  return material;
}

std::unique_ptr<SceneNode> SceneParser::ParseSceneNode() {
  auto node = make_unique<SceneNode>();
  std::string token;
  fs_ >> token;
  Assert(token, "{");
  while (token != "}") {
    fs_ >> token;
    if (token == "Node") {
      node->AddChild(ParseSceneNode());
    } else if (token == "Transform") {
      ParseTransform(node->GetTransform());
    } else if (token.find("Component") != std::string::npos) {
      size_t begin = token.find("<") + 1;
      size_t end = token.find(">");
      if (begin == std::string::npos || end == std::string::npos)
        throw std::runtime_error("Bad format of Component<*>!");
      std::string type = token.substr(begin, end - begin);
      ParseComponent(type, *node);
    } else if (token != "}") {
      throw std::runtime_error("Bad node token: " + token + "!");
    }
  }

  return node;
}

void SceneParser::ParseTransform(Transform& transform) {
  std::string token;
  fs_ >> token;
  Assert(token, "{");

  glm::mat4 T(1.0f);
  while (token != "}") {
    fs_ >> token;
    if (token == "translate") {
      T = glm::translate(T, ReadVec3());
    } else if (token == "x_rotate") {
      float angle = ToRadian(ReadFloat());
      T = glm::rotate(T, angle, glm::vec3(1.0f, 0.0f, 0.0f));
    } else if (token == "y_rotate") {
      float angle = ToRadian(ReadFloat());
      T = glm::rotate(T, angle, glm::vec3(0.0f, 1.0f, 0.0f));
    } else if (token == "z_rotate") {
      float angle = ToRadian(ReadFloat());
      T = glm::rotate(T, angle, glm::vec3(0.0f, 0.0f, 1.0f));
    } else if (token == "scale") {
      T = glm::scale(T, ReadVec3());
    } else if (token != "}") {
      throw std::runtime_error("Bad transform token: " + token + "!");
    }
  }
  transform.SetMatrix4x4(T);
}

void SceneParser::ParseComponent(const std::string& type, SceneNode& node) {
  if (type == "Material") {
    ParseMaterialComponent(node);
  } else if (type == "Light") {
    ParseLightComponent(node);
  } else if (type == "Object") {
    ParseTracingComponent(node);
  } else {
    throw std::runtime_error("Bad component type: " + type + "!");
  }
}

void SceneParser::ParseCamera() {
  std::string token;
  fs_ >> token;
  Assert(token, "{");

  while (token != "}") {
    fs_ >> token;
    if (token == "center") {
      camera_spec_.center = ReadVec3();
    } else if (token == "direction") {
      camera_spec_.direction = ReadVec3();
    } else if (token == "up") {
      camera_spec_.up = ReadVec3();
    } else if (token == "fov") {
      camera_spec_.fov = ReadFloat();
    } else if (token != "}") {
      throw std::runtime_error("Bad camera token: " + token + "!");
    }
  }
}

void SceneParser::ParseMaterialComponent(SceneNode& node) {
  std::string token;
  fs_ >> token;
  Assert(token, "{");
  fs_ >> token;
  Assert(token, "index");
  int idx = ReadInt();
  fs_ >> token;
  Assert(token, "}");

  node.CreateComponent<MaterialComponent>(materials_.at(idx));
}

void SceneParser::ParseLightComponent(SceneNode& node) {
  std::string token;
  fs_ >> token;
  Assert(token, "{");

  std::string type;
  glm::vec3 color;
  float attenuation = 20.0f;
  glm::vec3 direction;

  std::shared_ptr<LightBase> light;
  while (token != "}") {
    fs_ >> token;
    if (token == "type") {
      fs_ >> type;
    } else if (token == "color") {
      color = ReadVec3();
    } else if (token == "direction") {
      direction = glm::normalize(ReadVec3());
    } else if (token == "attenuation") {
      attenuation = ReadFloat();
    } else if (token != "}") {
      throw std::runtime_error("Bad light token: " + token + "!");
    }
  }
  if (type == "point") {
    auto point_light = std::make_shared<PointLight>();
    point_light->SetDiffuseColor(color);
    point_light->SetSpecularColor(color);
    point_light->SetAttenuation(glm::vec3(attenuation));
    light = std::move(point_light);
  } else if (type == "directional") {
    auto directional_light = std::make_shared<DirectionalLight>();
    directional_light->SetDiffuseColor(color);
    directional_light->SetSpecularColor(color);
    directional_light->SetDirection(direction);
    light = std::move(directional_light);
  } else {
    throw std::runtime_error("Bad light type: " + type + "!");
  }
  node.CreateComponent<LightComponent>(std::move(light));
}

void SceneParser::ParseTracingComponent(SceneNode& node) {
  std::string token;
  fs_ >> token;
  Assert(token, "{");

  std::shared_ptr<HittableBase> object;
  fs_ >> token;
  Assert(token, "type");
  std::string type;
  fs_ >> type;
  if (type == "sphere") {
    fs_ >> token;
    Assert(token, "radius");
    float radius = ReadFloat();
    object = std::make_shared<Sphere>(radius);
    fs_ >> token;
    Assert(token, "}");
  } else if (type == "plane") {
    glm::vec3 normal;
    float offset;
    while (true) {
      fs_ >> token;
      if (token == "normal") {
        normal = ReadVec3();
      } else if (token == "offset") {
        offset = ReadFloat();
      } else if (token == "}") {
        break;
      } else {
        throw std::runtime_error("Bad plane token: " + token + "!");
      }
    }
    object = std::make_shared<Plane>(normal, offset);
  } else if (type == "triangle") {
    glm::vec3 v0, v1, v2;
    fs_ >> token;
    Assert(token, "vertex0");
    v0 = ReadVec3();
    fs_ >> token;
    Assert(token, "vertex1");
    v1 = ReadVec3();
    fs_ >> token;
    Assert(token, "vertex2");
    v2 = ReadVec3();
    fs_ >> token;
    Assert(token, "}");
    glm::vec3 n = glm::normalize(glm::cross(v1 - v0, v2 - v0));
    object = std::make_shared<Triangle>(v0, v1, v2, n, n, n);
  } else if (type == "mesh") {
    std::string filename;
    fs_ >> token;
    Assert(token, "obj_file");
    fs_ >> filename;
    fs_ >> token;
    Assert(token, "}");
    bool success;
    auto data = ObjParser::Parse(base_path_ + filename, success);
    if (!success || data.positions == nullptr || data.indices == nullptr) {
      throw std::runtime_error("Failed at parsing " + filename);
    }
    if (data.normals == nullptr) {
      data.normals = CalculateNormals(*data.positions, *data.indices);
    }
    object = std::make_shared<Mesh>(std::move(data.positions),
                                    std::move(data.normals),
                                    std::move(data.indices));
  } else {
    throw std::runtime_error("Bad object type: " + type + "!");
  }

  node.CreateComponent<TracingComponent>(std::move(object));
}

glm::vec3 SceneParser::ReadVec3() {
  float r, g, b;
  if (!(fs_ >> r >> g >> b)) {
    throw std::runtime_error("Error in ReadVec3()");
  }
  return glm::vec3(r, g, b);
}

float SceneParser::ReadFloat() {
  float x;
  if (!(fs_ >> x)) {
    throw std::runtime_error("Error in ReadFloat()");
  }
  return x;
}

int SceneParser::ReadInt() {
  int x;
  if (!(fs_ >> x)) {
    throw std::runtime_error("Error in ReadInt()");
  }
  return x;
}
}  // namespace GLOO
