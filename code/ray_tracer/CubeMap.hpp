#ifndef CUBE_MAP_H_
#define CUBE_MAP_H_

#include <string>
#include <iostream>

#include <glm/glm.hpp>

#include "gloo/Image.hpp"

namespace GLOO {
class CubeMap {
 public:
  // Assumes a directory containing {left,right,up,down,front,back}.png.
  CubeMap(const std::string& directory);

  // Returns color for given directory
  glm::vec3 GetTexel(const glm::vec3& direction) const;

 private:
  // The UV (x, y) coordinates are assumed to be normalized between 0 and 1.
  // The resulting look up is box filtered in the local 2x2 neighborhood.
  glm::vec3 GetFaceTexel(float x, float y, int face) const;
  const glm::vec3& GetTexturePixel(int x, int y, int face) const;

  std::unique_ptr<Image> images_[6];
};
}  // namespace GLOO

#endif
