#pragma once

#include <cstdint>

namespace GuiSE {
enum class OpCode : uint8_t {
  NoOp,
  Constant,
  Add,
  Multiply,
  Divide,
  Negate,
  Log,
  Return,
};
} // namespace GuiSE
