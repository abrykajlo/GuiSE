#pragma once

#include <cstdint>

namespace GuiSE {
enum class OpCode : uint8_t {
  NoOp,
  Constant,
  Global,
  GetGlobal,
  GetLocal,
  SetGlobal,
  SetLocal,
  Call,
  StackUp,
  Pop,
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
  TypeArg,
  Return,
};
} // namespace GuiSE
