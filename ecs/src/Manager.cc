#pragma once

#include "ecs/Manager.h"
#include <cassert>

namespace ecs {
namespace {
constexpr Entity::HandleType kMinimumFreeIndices = 1024;
}  // namespace

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
  Entity::HandleType idx;
  if (free_indices_.size() > kMinimumFreeIndices) {
    idx = free_indices_.front();
    free_indices_.pop();
  } else {
    generations_.push_back(0);
    idx = generations_.size() - 1;
    assert(idx < ((Entity::HandleType)1 << Entity::kIndexBits));
  }
  return Entity(idx, generations_[idx]);
}

void Manager::Destroy(Entity entity) {
  // Destroy the components.
  size_t idx = 0;
  for (auto it : component_destroy_) {
    if (it) {
      it(component_pools_[idx++], entity);
    }
  }

  // Destroy the handle.
  ++generations_[entity.Index()];
  free_indices_.push(entity.Index());
}

bool Manager::IsAlive(Entity entity) const {
  return generations_[entity.Index()] ==
         static_cast<uint8_t>(entity.Generation());
}
}  // namespace ecs