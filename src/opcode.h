#pragma once

#include "types.h"

#include <cstdint>
#include <functional>

namespace GuiSE {
enum class OpCode : u8 {
  NoOp,
  Push,
  Add,
  Multiply,
  Divide,
  Negate,
  Return,
};

#pragma pack(push, 1)
template <OpCode O> struct Instruction {
  OpCode op_code;
};

template <> struct Instruction<OpCode::Push> {
  OpCode op_code;
  f64 value;
};
#pragma pack(pop)

union InstructionSet {
  OpCode op_code;
  Instruction<OpCode::NoOp> no_op;
  Instruction<OpCode::Push> op_push;
  Instruction<OpCode::Add> op_add;
  Instruction<OpCode::Multiply> op_multiply;
  Instruction<OpCode::Divide> op_divide;
  Instruction<OpCode::Negate> op_negate;
  Instruction<OpCode::Return> op_return;
};

template <OpCode O> inline auto make_instruction() { return Instruction<O>{O}; }
} // namespace GuiSE
