#pragma once

#include "chunk.h"

#define STACK_MAX 256

namespace GuiSE {
enum class InterpretResult { OK, COMPILE_ERROR, RUNTIME_ERROR };

class VM {
public:
  InterpretResult interpret(Chunk *chunk);
  InterpretResult run();

  inline u8 read() { return *_ip++; }

private:
  void resetStack();
  void push(Value value);
  Value pop();

  Chunk *_chunk = nullptr;
  const u8 *_ip = nullptr;
  Value _stack[STACK_MAX];
  Value *_stackTop;
};
} // namespace GuiSE