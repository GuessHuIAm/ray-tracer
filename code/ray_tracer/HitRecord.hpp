#ifndef HIT_RECORD_H_
#define HIT_RECORD_H_

#include <limits>
#include <ostream>

#include <glm/gtx/string_cast.hpp>

#include "gloo/Material.hpp"

namespace GLOO {
struct HitRecord {
  HitRecord() {
    time = std::numeric_limits<float>::max();
  }

  float time;
  glm::vec3 normal;
};

inline std::ostream& operator<<(std::ostream& os, const HitRecord& rec) {
  os << "HitRecord <" << rec.time << ", " << glm::to_string(rec.normal) << ">";
  return os;
}

}  // namespace GLOO

#endif
