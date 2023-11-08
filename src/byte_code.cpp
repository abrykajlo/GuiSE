#include "byte_code.h"

#include "opcode.h"
#include "types.h"

using namespace GuiSE;

void GuiSE::ByteCode::Write(uint8_t byte) {}

int GuiSE::ByteCode::AddConstant(Value value) {
  _constants.push_back(value);
  return _constants.size();
}

Value GuiSE::ByteCode::GetConstant(int index) const {
  return _constants[index];
}

const std::vector<uint8_t> &ByteCode::get_byte_code() const {
  return _byte_code;
}
