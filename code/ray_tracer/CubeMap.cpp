#include "CubeMap.hpp"

#include <cmath>
#include <string>
#include <iostream>
#include <algorithm>

namespace {
enum FACE {
  LEFT,
  RIGHT,
  UP,
  DOWN,
  FRONT,
  BACK,
};
}

namespace GLOO {
CubeMap::CubeMap(const std::string& directory) {
  std::string side[6] = {"left", "right", "up", "down", "front", "back"};
  for (int i = 0; i < 6; i++) {
    std::string filename = directory + "/" + side[i] + ".png";
    images_[i] = Image::LoadPNG(filename, false);
  }
}

glm::vec3 CubeMap::GetFaceTexel(float x, float y, int face) const {
  x = x * images_[face]->GetWidth();
  y = (1 - y) * images_[face]->GetHeight();
  int ix = (int)x;
  int iy = (int)y;
  float alpha = x - ix;
  float beta = y - iy;

  const glm::vec3& pixel0 = GetTexturePixel(ix + 0, iy + 0, face);
  const glm::vec3& pixel1 = GetTexturePixel(ix + 1, iy + 0, face);
  const glm::vec3& pixel2 = GetTexturePixel(ix + 0, iy + 1, face);
  const glm::vec3& pixel3 = GetTexturePixel(ix + 1, iy + 1, face);

  glm::vec3 color;
  for (int i = 0; i < 3; i++) {
    color[i] = (1 - alpha) * (1 - beta) * pixel0[i] +
               alpha * (1 - beta) * pixel1[i] + (1 - alpha) * beta * pixel2[i] +
               alpha * beta * pixel3[i];
  }

  return color;
}

glm::vec3 CubeMap::GetTexel(const glm::vec3& direction) const {
  glm::vec3 dir = glm::normalize(direction);
  glm::vec3 outputColor(0.0f);
  if ((std::abs(dir[0]) >= std::abs(dir[1])) &&
      (std::abs(dir[0]) >= std::abs(dir[2]))) {
    if (dir[0] > 0.0f) {
      outputColor = GetFaceTexel((dir[2] / dir[0] + 1.0f) * 0.5f,
                                 (dir[1] / dir[0] + 1.0f) * 0.5f, RIGHT);
    } else if (dir[0] < 0.0f) {
      outputColor = GetFaceTexel((dir[2] / dir[0] + 1.0f) * 0.5f,
                                 1.0f - (dir[1] / dir[0] + 1.0f) * 0.5f, LEFT);
    }
  } else if ((std::abs(dir[1]) >= std::abs(dir[0])) &&
             (std::abs(dir[1]) >= std::abs(dir[2]))) {
    if (dir[1] > 0.0f) {
      outputColor = GetFaceTexel((dir[0] / dir[1] + 1.0f) * 0.5f,
                                 (dir[2] / dir[1] + 1.0f) * 0.5f, UP);
    } else if (dir[1] < 0.0f) {
      outputColor = GetFaceTexel(1.0f - (dir[0] / dir[1] + 1.0f) * 0.5f,
                                 1.0f - (dir[2] / dir[1] + 1.0f) * 0.5f, DOWN);
    }
  } else if ((std::abs(dir[2]) >= std::abs(dir[0])) &&
             (std::abs(dir[2]) >= std::abs(dir[1]))) {
    if (dir[2] > 0.0f) {
      outputColor = GetFaceTexel(1.0f - (dir[0] / dir[2] + 1.0f) * 0.5f,
                                 (dir[1] / dir[2] + 1.0f) * 0.5f, FRONT);
    } else if (dir[2] < 0.0f) {
      outputColor = GetFaceTexel((dir[0] / dir[2] + 1.0f) * 0.5f,
                                 1.0f - (dir[1] / dir[2] + 1.0f) * 0.5f, BACK);
    }
  }

  return outputColor;
}

const glm::vec3& CubeMap::GetTexturePixel(int x, int y, int face) const {
  x = std::min(std::max(0, x), (int)(images_[face]->GetWidth() - 1));
  y = std::min(std::max(0, y), (int)(images_[face]->GetHeight() - 1));
  return images_[face]->GetPixel(x, y);
}
}  // namespace GLOO
