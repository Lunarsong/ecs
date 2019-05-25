#pragma once

#include <ecs/Handle.h>

namespace ecs {

class Entity {
 public:
  Entity() = default;

  Handle Handle() const;

 private:
  ecs::Handle handle_;

  friend class Manager;
};

}  // namespace ecs