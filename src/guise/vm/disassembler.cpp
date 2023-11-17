#include "disassembler.h"

#include <guise/compiler/types.h>
#include <guise/vm/byte_code.h>
#include <guise/vm/opcode.h>

#include <cstdio>

namespace {
int simple_instruction(const char *name, int offset) {
  printf("%s\n", name);
  return offset + 1;
}
} // namespace

using namespace GuiSE;

Disassembler::Disassembler(const char *name, const ByteCode &byte_code)
    : _byte_code(byte_code), _name(name) {}

void Disassembler::Disassemble() const {
  printf("== %s ==\n", _name);

  for (int offset = 0; offset < _byte_code.get_byte_code().size();) {
    offset = _disassemble_instruction(offset);
  }
}

uint8_t Disassembler::_get_byte(int offset) const {
  return _byte_code.get_byte_code()[offset];
}

int Disassembler::_disassemble_instruction(int offset) const {
  printf("%04d ", offset);

  uint8_t byte = _get_byte(offset);
  switch (static_cast<OpCode>(byte)) {
  case OpCode::Constant:
    return _constant_instruction("PUSH", offset);
  case OpCode::Add:
    return simple_instruction("ADD", offset);
  case OpCode::Multiply:
    return simple_instruction("MULTIPLY", offset);
  case OpCode::Divide:
    return simple_instruction("DIVIDE", offset);
  case OpCode::Negate:
    return simple_instruction("NEGATE", offset);
  case OpCode::Return:
    return simple_instruction("RETURN", offset);
  default:
    printf("Unknown opcode %d\n", byte);
    return offset + 1;
  }
}

int Disassembler::_constant_instruction(const char *name, int offset) const {
  const int index = _get_byte(offset + 1);
  printf("%-16s %g\n", name, _byte_code.GetConstant(index));
  return offset + 2;
}