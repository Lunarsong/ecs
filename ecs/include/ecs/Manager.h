#pragma once

#include <queue>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>
#include "ComponentStorage.h"
#include "Entity.h"

namespace ecs {

class Manager {
 public:
  ~Manager();

  Entity Create();
  void Destroy(Entity entity);
  bool IsAlive(Entity entity) const;

  template <typename Component, typename... Args>
  Component& Assign(Entity entity, Args&&... args);
  template <typename Component>
  Component& Get(Entity entity);
  template <typename Component>
  void Remove(Entity entity);

  template <typename Component, typename Fn>
  void ForEach(Fn fn);
  template <typename... Components, typename Fn,
            typename std::enable_if<1 != sizeof...(Components), int>::type = 0>
  void ForEach(Fn fn);

 private:
  // Handle generation.
  std::queue<HandleType> free_indices_;
  std::vector<uint8_t> generations_;

  // Components.
  using VoidFn = void (*)(void*);
  using EntityFn = void (*)(void*, Entity);
  std::vector<void*> component_pools_;
  std::vector<EntityFn> component_destroy_;
  std::vector<VoidFn> pool_destroy_;
  static size_t families_counter_;

  template <typename Component>
  size_t ComponentFamily() {
    static size_t family = Manager::families_counter_++;
    return family;
  }

  template <typename Component>
  ComponentStorage<Component>* GetPool();
};

template <typename Component, typename... Args>
Component& Manager::Assign(Entity entity, Args&&... args) {
  return GetPool<Component>()->Assign(entity, std::forward<Args>(args)...);
}
template <typename Component>
Component& Manager::Get(Entity entity) {
  return *GetPool<Component>()->Get(entity);
}
template <typename Component>
void Manager::Remove(Entity entity) {
  GetPool<Component>()->Remove(entity);
}

template <typename Component>
ComponentStorage<Component>* Manager::GetPool() {
  size_t family = ComponentFamily<Component>();
  if (component_pools_.size() <= family) {
    component_pools_.resize(family + 1, nullptr);
    pool_destroy_.resize(family + 1, nullptr);
    component_destroy_.resize(family + 1, nullptr);
  }
  if (!component_pools_[family]) {
    component_pools_[family] = new ComponentStorage<Component>();
    component_destroy_[family] = [](void* storage, Entity entity) {
      reinterpret_cast<ComponentStorage<Component>*>(storage)->Remove(entity);
    };
    pool_destroy_[family] = [](void* storage) {
      auto* cast_storage =
          reinterpret_cast<ComponentStorage<Component>*>(storage);
      cast_storage->Clean();
      delete cast_storage;
    };
  }
  return reinterpret_cast<ComponentStorage<Component>*>(
      component_pools_[family]);
}

template <typename Component, typename Fn>
void Manager::ForEach(Fn fn) {
  auto& pool = *GetPool<Component>();
  for (ComponentStorage<Component>::Iterator it = pool.begin();
       it != pool.end(); ++it) {
    fn(it.GetEntity(), *it);
  }
}

template <typename F, typename Tuple, size_t... S>
decltype(auto) apply_tuple_impl(Entity e, F&& fn, Tuple&& t,
                                std::index_sequence<S...>) {
  return std::forward<F>(fn)(e, *std::get<S>(std::forward<Tuple>(t))...);
}
template <typename F, typename Tuple>
decltype(auto) apply_from_tuple(Entity e, F&& fn, Tuple&& t) {
  std::size_t constexpr tSize =
      std::tuple_size<typename std::remove_reference<Tuple>::type>::value;
  return apply_tuple_impl(e, std::forward<F>(fn), std::forward<Tuple>(t),
                          std::make_index_sequence<tSize>());
}

template <typename Component, typename StorageTuple>
void FindSmallestHelper(StorageTuple storages, size_t& size, void** ptr) {
  auto* pool = std::get<ComponentStorage<Component>*>(storages);
  if (pool->Size() < size) {
    *ptr = pool;
    size = pool->Size();
  }
}

template <typename Component, typename Fn, typename StorageTuple>
void IterateHelper(StorageTuple storages, Fn fn, void* ptr) {
  auto* pool = std::get<ComponentStorage<Component>*>(storages);
  if (pool == ptr) {
    for (auto it = pool->begin(); it != pool->end(); ++it) {
      fn(it.GetEntity());
    }
  }
}

template <typename Component, typename ComponentsTuple>
void IsComponentMissingHelper(ComponentsTuple components, bool& has_all) {
  if (std::get<Component*>(components) == nullptr) {
    has_all = false;
  }
}

template <typename... Components, typename Fn,
          typename std::enable_if<1 != sizeof...(Components), int>::type>
void Manager::ForEach(Fn fn) {
  // Create a tuple containing the component storage(s) of the requested
  // components.
  auto storages = std::make_tuple(GetPool<Components>()...);

  // Find the component storage with the fewest entities to use for iteration.
  size_t size = -1;
  void* ptr = nullptr;
  int find_smallest_expansion[] = {
      (FindSmallestHelper<Components>(storages, size, &ptr), 0)...};
  (void)find_smallest_expansion;

  // Helper lambda to expand tuple variables.
  auto iter_impl = [&](Entity e) {
    auto entities = std::make_tuple(GetPool<Components>()->Get(e)...);
    bool has_all_components = true;

    int component_missing_expansion[] = {
        (IsComponentMissingHelper<Components>(entities, has_all_components),
         0)...};
    (void)component_missing_expansion;

    if (has_all_components) {
      apply_from_tuple(e, fn, entities);
    }
  };

  // Iterate all entities in the smallest storage and call the helper lambda.
  int iterate_expansion[] = {
      (IterateHelper<Components>(storages, iter_impl, ptr), 0)...};
  (void)iterate_expansion;
}

}  // namespace ecs