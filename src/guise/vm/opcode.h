#pragma once

#include <cstdint>

namespace GuiSE {
enum class OpCode : uint8_t {
  NoOp,
  Constant,
  Add,
  Negate,
  Multiply,
  Divide,
  True,
  False,
  Not,
  Equal,
  Greater,
  Less,
  And,
  Or,
  Log,
  Return,
};
} // namespace GuiSE
