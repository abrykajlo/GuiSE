#include "chunk.h"

#include "opcode.h"

namespace {
int simpleInstruction(const char *name, int offset) {
  printf("%s\n", name);
  return offset + 1;
}
} // namespace

using namespace GuiSE;

void Chunk::write(u8 byte) { mCode.push_back(byte); }

std::optional<u8> Chunk::read(int offset) {
  if (offset < mCode.size()) {
    return mCode[offset];
  }

  return {};
}

const u8 *Chunk::getCodePtr() const {
  return mCode.empty() ? nullptr : mCode.data();
}

int Chunk::addConstant(Value value) {
  mConstants.push_back(value);
  return mConstants.size() - 1;
}

Value Chunk::getConstant(int index) const { return mConstants[index]; }

void Chunk::disassemble(const char *name) const {
  printf("== %s ==\n", name);

  for (int offset = 0; offset < mCode.size();) {
    offset = disassembleInstruction(offset);
  }
}

int Chunk::disassembleInstruction(int offset) const {
  printf("%04d ", offset);

  u8 instruction = mCode[offset];
  switch (static_cast<OpCode>(instruction)) {
  case OpCode::CONSTANT:
    return constantInstruction("CONSTANT", offset);
  case OpCode::RETURN:
    return simpleInstruction("RETURN", offset);
  default:
    printf("Unknown opcode %d\n", instruction);
    return offset + 1;
  }
}

int Chunk::constantInstruction(const char *name, int offset) const {
  u8 constant = mCode[offset + 1];
  printf("%-16s %4d '", name, constant);
  printValue(mConstants[constant]);
  printf("'\n");
  return offset + 2;
}
