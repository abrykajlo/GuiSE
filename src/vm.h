#pragma once

#include "chunk.h"

namespace GuiSE {
enum class InterpretResult { OK, COMPILE_ERROR, RUNTIME_ERROR };

class VM {
public:
  InterpretResult interpret(Chunk *chunk);
  InterpretResult run();

  template <class T> inline T readByte() { return static_cast<T>(*mIP++); }

  inline Value readConstant() { return mChunk->getConstant(readByte<u8>()); }

private:
  Chunk *mChunk = nullptr;
  const u8 *mIP = nullptr;
};
} // namespace GuiSE