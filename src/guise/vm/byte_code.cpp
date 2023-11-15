#include "byte_code.h"

#include "opcode.h"

#include <guise/compiler/types.h>

using namespace GuiSE;

void ByteCode::Write(uint8_t byte) { _byte_code.push_back(byte); }

int ByteCode::AddConstant(Value value) {
  _constants.push_back(value);
  return _constants.size() - 1;
}

Value ByteCode::GetConstant(int index) const { return _constants[index]; }

const std::vector<uint8_t> &ByteCode::get_byte_code() const {
  return _byte_code;
}
