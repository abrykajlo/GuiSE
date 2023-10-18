#pragma once

#include "chunk.h"

namespace GuiSE {
enum class InterpretResult { OK, COMPILE_ERROR, RUNTIME_ERROR };

class VM {
public:
  InterpretResult interpret(Chunk *chunk);
  InterpretResult run();

  inline u8 read() { return *_ip++; }

private:
  Chunk *_chunk = nullptr;
  const u8 *_ip = nullptr;
};
} // namespace GuiSE