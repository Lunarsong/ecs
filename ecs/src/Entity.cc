#include <ecs/Entity.h>

namespace ecs {

Entity::Entity(HandleType id) : id(id) {}
Entity::Entity(HandleType index, uint8_t generation) {
  id = (static_cast<HandleType>(generation) << kIndexBits) | index;
}

Entity::HandleType Entity::Id() const { return id; }
Entity::HandleType Entity::Index() const { return id & kIndexMask; }
Entity::HandleType Entity::Generation() const {
  return (id >> kIndexBits) & kGenerationMask;
}

}  // namespace ecs
