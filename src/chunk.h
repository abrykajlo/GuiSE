#pragma once

#include "types.h"
#include "value.h"

#include <cstdint>
#include <optional>
#include <vector>

namespace GuiSE {
class Chunk {
public:
  void write(u8 byte);
  std::optional<u8> read(int offset);

  const u8 *getCodePtr() const;

  int addConstant(Value value);
  Value getConstant(int index) const;

  void disassemble(const char *name) const;

private:
  int _disassembleInstruction(int offset) const;
  int _constantInstruction(const char *name, int offset) const;
  int _lineInstruction(const char *name, int offset) const;

  std::vector<u8> _code;
  ValueArray _constants;
};
} // namespace GuiSE