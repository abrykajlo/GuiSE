#include "chunk.h"

#include "opcode.h"

namespace {
int simpleInstruction(const char *name, int offset) {
  printf("%s\n", name);
  return offset + 1;
}
} // namespace

using namespace GuiSE;

void Chunk::write(u8 byte) { _code.push_back(byte); }

std::optional<u8> Chunk::read(int offset) {
  if (offset < _code.size()) {
    return _code[offset];
  }

  return {};
}

const u8 *Chunk::getCodePtr() const {
  return _code.empty() ? nullptr : _code.data();
}

int Chunk::addConstant(Value value) {
  _constants.push_back(value);
  return _constants.size() - 1;
}

Value Chunk::getConstant(int index) const { return _constants[index]; }

void Chunk::disassemble(const char *name) const {
  printf("== %s ==\n", name);

  for (int offset = 0; offset < _code.size();) {
    offset = _disassembleInstruction(offset);
  }
}

int Chunk::_disassembleInstruction(int offset) const {
  printf("%04d ", offset);

  u8 instruction = _code[offset];
  switch (static_cast<OpCode>(instruction)) {
  case OpCode::LINE:
    return _lineInstruction("LINE", offset);
  case OpCode::CONSTANT:
    return _constantInstruction("CONSTANT", offset);
  case OpCode::RETURN:
    return simpleInstruction("RETURN", offset);
  default:
    printf("Unknown opcode %d\n", instruction);
    return offset + 1;
  }
}

int Chunk::_constantInstruction(const char *name, int offset) const {
  u8 constant = _code[offset + 1];
  printf("%-16s %4d '", name, constant);
  printValue(_constants[constant]);
  printf("'\n");
  return offset + 2;
}

int Chunk::_lineInstruction(const char *name, int offset) const {
  u8 line = _code[offset + 1];
  printf("%s %d\n", name, line);
  return offset + 2;
}
