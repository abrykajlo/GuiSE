#include "byte_code.h"
#include "opcode.h"
#include "vm.h"

using namespace GuiSE;

int main(int argc, const char *argv[]) {
  VM vm;
  ByteCode byte_code;
  auto push = make_instruction<OpCode::Push>();
  push.value = 1;
  byte_code.Write(push);
  push.value = 2;
  byte_code.Write(push);
  byte_code.Write(make_instruction<OpCode::Add>());
  byte_code.Write(make_instruction<OpCode::Return>());
  byte_code.Disassemble("test chunk");
  vm.Interpret(&byte_code);
  return 0;
}