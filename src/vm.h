#pragma once

#include "byte_code.h"

#define STACK_MAX 256

namespace GuiSE {
enum class InterpretResult { Ok, CompileError, RuntimeError };

class VM {
public:
  InterpretResult Interpret(const ByteCode &byte_code);
  InterpretResult Interpret(const char *source);

private:
  InterpretResult _run();
  template <OpCode O> inline void _read() { _ip += sizeof(Instruction<O>); }

  inline auto _peek() { return reinterpret_cast<const InstructionSet *>(_ip); }

  void _reset_stack();
  void _push(Number value);
  Number _pop();

  const ByteCode *_byte_code = nullptr;
  const uint8_t *_ip = nullptr;
  uint8_t _stack[STACK_MAX];
  uint8_t *_stack_top;
};
} // namespace GuiSE