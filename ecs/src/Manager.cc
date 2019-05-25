#pragma once

#include "ecs/Manager.h"

namespace ecs {
size_t Manager::families_counter_ = 0;

Manager::~Manager() {
  size_t idx = 0;
  for (auto it : pool_destroy_) {
    if (it) {
      it(component_pools_[idx++]);
    }
  }
}

Entity Manager::Create() {
  static size_t idx = 0;
  Entity entity;
  entity.handle_.id = idx++;
  return entity;
}
void Manager::Destroy(Entity entity) {
  size_t idx = 0;
  for (auto it : component_destroy_) {
    if (it) {
      it(component_pools_[idx++], entity);
    }
  }
}
}  // namespace ecs