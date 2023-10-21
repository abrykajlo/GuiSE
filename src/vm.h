#pragma once

#include "byte_code.h"

#define STACK_MAX 256

namespace GuiSE {
enum class InterpretResult { Ok, CompileError, RuntimeError };

class VM {
public:
  InterpretResult Interpret(ByteCode *byte_code);
  InterpretResult Run();

private:
	template <OpCode O> inline void _read() { _ip += sizeof(Instruction<O>); }

  inline auto _peek() { return reinterpret_cast<const InstructionSet *>(_ip); }

  void _reset_stack();
  void _push(f64 value);
  f64 _pop();

  ByteCode *_byte_code = nullptr;
  const u8 *_ip = nullptr;
  u8 _stack[STACK_MAX];
  u8 *_stack_top;
};
} // namespace GuiSE