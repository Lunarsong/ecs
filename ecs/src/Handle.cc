#include <ecs/Handle.h>

#include <cassert>

namespace ecs {
namespace {
constexpr HandleType kGenerationBits = 8;
constexpr HandleType kGenerationMask = (1 << kGenerationBits) - 1;
constexpr HandleType kIndexBits = (sizeof(HandleType) * 8) - kGenerationBits;
constexpr HandleType kIndexMask = (HandleType(1) << kIndexBits) - 1;
constexpr HandleType kMinimumFreeIndices = 1024;
}  // namespace

Handle::Handle(HandleType index, uint8_t generation) {
  id = (static_cast<HandleType>(generation) << kIndexBits) | index;
}
Handle::Handle(HandleType id) : id(id) {}
HandleType Handle::Index() const { return id & kIndexMask; }
HandleType Handle::Generation() const {
  return (id >> kIndexBits) & kGenerationMask;
}

}  // namespace ecs