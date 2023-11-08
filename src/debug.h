#pragma once

#include <cassert>
#include <cstdint>

#ifdef _DEBUG
#define GUISE_ASSERT(ASSERTION) assert(ASSERTION);
#else
#define GUISE_ASSERT(ASSERTION)
#endif

namespace GuiSE {

class ByteCode;

struct Value;

class Disassembler {
public:
  Disassembler(const char *name, const ByteCode &byte_code);

  void Disassemble() const;

private:
  uint8_t _get_byte(int offset) const;
  int _disassemble_instruction(int offset) const;
  int _constant_instruction(const char *name, int offset) const;

  const ByteCode &_byte_code;
  const char *_name;
};
} // namespace GuiSE