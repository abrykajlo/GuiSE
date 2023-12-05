#include "byte_code.h"

#include "opcode.h"

#include <guise/compiler/types.h>

using namespace GuiSE;

void ByteCode::Write(uint8_t byte) { _byte_code.push_back(byte); }

void ByteCode::AddFunction(const std::string &function_name, int fn_ptr) {
  _functions.insert({function_name, fn_ptr});
}

const uint8_t *ByteCode::GetFunction(const std::string &function_name) const {
  auto it = _functions.find(function_name);
  if (it != _functions.end()) {
    return &_byte_code[it->second];
  }
  return nullptr;
}

int ByteCode::AddConstant(Value value) {
  _constants.push_back(value);
  return _constants.size() - 1;
}

Value ByteCode::GetConstant(int index) const { return _constants[index]; }

size_t ByteCode::Length() const { return _byte_code.size(); }

const uint8_t *ByteCode::operator[](const size_t i) const {
  if (i >= Length()) {
    return nullptr;
  }
  return &_byte_code[i];
}
