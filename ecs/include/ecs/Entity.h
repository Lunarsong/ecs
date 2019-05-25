#pragma once

#include <cstdint>

namespace ecs {

class Entity {
 public:
  using HandleType = uint64_t;
  static constexpr HandleType kGenerationBits = 8;
  static constexpr HandleType kGenerationMask = (1 << kGenerationBits) - 1;
  static constexpr HandleType kIndexBits =
      (sizeof(HandleType) * 8) - kGenerationBits;
  static constexpr HandleType kIndexMask = (HandleType(1) << kIndexBits) - 1;
  static constexpr HandleType kInvalidHandle = 0;

  Entity() = default;

  HandleType Id() const;
  HandleType Index() const;
  HandleType Generation() const;

 private:
  HandleType id = kInvalidHandle;

  friend class Manager;
  Entity(HandleType id);
  Entity(HandleType index, uint8_t generation);
};

}  // namespace ecs