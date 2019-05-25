#pragma once

#include <cstdint>

namespace ecs {

using HandleType = uint64_t;
constexpr HandleType kInvalidHandle = 0;

struct Handle {
  HandleType id = kInvalidHandle;

  Handle() = default;
  Handle(HandleType id);
  Handle(HandleType index, uint8_t generation);

  HandleType Index() const;
  HandleType Generation() const;
};

}  // namespace ecs