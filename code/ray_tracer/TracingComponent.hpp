#ifndef TRACING_COMPONENT_H_
#define TRACING_COMPONENT_H_

#include "gloo/components/ComponentBase.hpp"
#include "gloo/components/ComponentType.hpp"

#include "hittable/HittableBase.hpp"

namespace GLOO {
class TracingComponent : public ComponentBase {
 public:
  TracingComponent(std::shared_ptr<HittableBase> hittable)
      : _hittable(std::move(hittable)) {
  }
  const HittableBase& GetHittable() const {
    return *_hittable;
  }

 private:
  std::shared_ptr<HittableBase> _hittable;
};

CREATE_COMPONENT_TRAIT(TracingComponent, ComponentType::Tracing);
}  // namespace GLOO

#endif
