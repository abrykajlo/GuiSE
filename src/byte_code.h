#pragma once

#include "opcode.h"
#include "types.h"

#include <cstdint>
#include <optional>
#include <vector>

namespace GuiSE {
class ByteCode {
public:
  template <OpCode O> void Write(const Instruction<O> &instruction) {
    auto byte = reinterpret_cast<const char *>(&instruction);
    for (int i = 0; i < sizeof(Instruction<O>); i++) {
      _byte_code.push_back(*(byte + i));
    }
  }

  const uint8_t *GetByteCodePtr() const;

  void Disassemble(const char *name) const;

private:
  int _disassemble_instruction(int offset) const;
  int _push_instruction(const char *name,
                        const Instruction<OpCode::Push> &instruction,
                        int offset) const;

  std::vector<uint8_t> _byte_code;
};
} // namespace GuiSE