#pragma once

#include <algorithm>
#include <cassert>
#include <vector>
#include "Entity.h"

namespace ecs {
template <typename Component>
class ComponentStorage {
 public:
  Component& Assign(Entity entity, Component&& component);
  template <typename... Args>
  Component& Assign(Entity entity, Args&&... args);
  void Remove(Entity entity);
  void Assure(Entity entity);
  Component* Get(Entity entity);
  bool Has(Entity entity);

  size_t Size() const;
  void Clean();

 private:
  using ArrayIndex = size_t;
  static constexpr ArrayIndex kInvalidIndex = -1;
  std::vector<ArrayIndex> sparse_to_packed_;
  std::vector<Entity> packed_to_sparse_;
  std::vector<Component> packed_;

 public:
  class Iterator {
   public:
    using ComponentIteratorType = typename std::vector<Component>::iterator;
    using EntityIteratorType = std::vector<Entity>::iterator;

    Iterator(ComponentIteratorType component_iter,
             EntityIteratorType entity_iter)
        : component_iter_(component_iter), entity_iter_(entity_iter) {}

    Entity GetEntity() { return *entity_iter_; }
    Component& operator*() { return *component_iter_; }
    Component* operator->() { return &*component_iter_; }
    Iterator& operator++() {
      ++component_iter_;
      ++entity_iter_;
      return *this;
    }
    bool operator==(const Iterator& other) const {
      return component_iter_ == other.component_iter_;
    }
    bool operator!=(const Iterator& other) const {
      return component_iter_ != other.component_iter_;
    }

   private:
    ComponentIteratorType component_iter_;
    EntityIteratorType entity_iter_;
  };

  Iterator begin() {
    return ComponentStorage<Component>::Iterator(packed_.begin(),
                                                 packed_to_sparse_.begin());
  }
  Iterator end() {
    return ComponentStorage<Component>::Iterator(packed_.end(),
                                                 packed_to_sparse_.end());
  }
};

template <typename Component>
Component& ComponentStorage<Component>::Assign(Entity entity,
                                               Component&& component) {
  Assure(entity);
  const size_t entity_index = entity.Index();

  ArrayIndex packed_index = sparse_to_packed_[entity_index];
  if (packed_index == kInvalidIndex) {
    packed_index = packed_.size();
    packed_to_sparse_.emplace_back(entity);
    packed_.emplace_back(component);
    sparse_to_packed_[entity_index] = packed_index;
  } else {
    packed_[packed_index] = component;
  }
  return packed_[packed_index];
}

template <typename Component>
template <typename... Args>
Component& ComponentStorage<Component>::Assign(Entity entity, Args&&... args) {
  Assure(entity);
  const size_t entity_index = entity.Index();

  ArrayIndex packed_index = sparse_to_packed_[entity_index];
  if (packed_index == kInvalidIndex) {
    packed_index = packed_.size();
    packed_to_sparse_.emplace_back(entity);
    packed_.emplace_back(std::forward<Args>(args)...);
    sparse_to_packed_[entity_index] = packed_index;
  } else {
    packed_[packed_index] = Component(std::forward<Args>(args)...);
  }
  return packed_[packed_index];
}

template <typename Component>
void ComponentStorage<Component>::Remove(Entity entity) {
  const size_t entity_index = entity.Index();
  if (entity.Index() < sparse_to_packed_.size()) {
    return;
  }

  const ArrayIndex packed_index = sparse_to_packed_[entity_index];
  if (packed_index == kInvalidIndex) {
    return;
  }
  const size_t last_index = packed_.size() - 1;
  const Entity last_entity = packed_to_sparse_[last_index];
  sparse_to_packed_[entity_index] = kInvalidIndex;
  sparse_to_packed_[last_entity.Index()] = packed_index;
  std::swap(packed_[packed_index], packed_[last_index]);
  packed_.pop_back();
  std::swap(packed_to_sparse_[packed_index], packed_to_sparse_[last_index]);
  packed_to_sparse_.pop_back();
}

template <typename Component>
void ComponentStorage<Component>::Assure(Entity entity) {
  const size_t entity_index = entity.Index();
  if (sparse_to_packed_.size() <= entity_index) {
    sparse_to_packed_.resize(entity_index + 1, kInvalidIndex);
  }
}

template <typename Component>
Component* ComponentStorage<Component>::Get(Entity entity) {
  if (Has(entity)) {
    const size_t entity_index = entity.Index();
    return &packed_[sparse_to_packed_[entity_index]];
  }
  return nullptr;
}

template <typename Component>
bool ComponentStorage<Component>::Has(Entity entity) {
  const size_t entity_index = entity.Index();
  return (entity_index < sparse_to_packed_.size() &&
          sparse_to_packed_[entity_index] != kInvalidIndex);
}

template <typename Component>
size_t ComponentStorage<Component>::Size() const {
  return packed_.size();
}

template <typename Component>
void ComponentStorage<Component>::Clean() {
  packed_.clear();
  packed_to_sparse_.clear();
  sparse_to_packed_.clear();
}

}  // namespace ecs