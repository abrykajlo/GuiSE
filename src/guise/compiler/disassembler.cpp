#include "disassembler.h"

#include <guise/compiler/types.h>
#include <guise/vm/byte_code.h>
#include <guise/vm/opcode.h>

#include <cstdio>

using namespace GuiSE;

namespace {
void simple_instruction(const char *name, const uint8_t *&instruction) {
  printf("%s\n", name);
  instruction++;
}

void arg_instruction(const char *name, const uint8_t *&instruction) {
  const uint8_t arg = *(instruction + 1);
  printf("%-16s %d\n", name, arg);
  instruction += 2;
}

void disassemble_instruction(const uint8_t *&instruction) {
  uint8_t byte = *instruction;
  switch (static_cast<OpCode>(byte)) {
  case OpCode::NoOp:
    simple_instruction("NO_OP", instruction);
    return;
  case OpCode::Constant:
    arg_instruction("CONSTANT", instruction);
    return;
  case OpCode::Global:
    simple_instruction("GLOBAL", instruction);
    return;
  case OpCode::GetGlobal:
    arg_instruction("GET_GLOBAL", instruction);
    return;
  case OpCode::GetLocal:
    arg_instruction("GET_LOCAL", instruction);
    return;
  case OpCode::SetGlobal:
    arg_instruction("SET_GLOBAL", instruction);
    return;
  case OpCode::SetLocal:
    arg_instruction("SET_LOCAL", instruction);
    return;
  case OpCode::Call:
    arg_instruction("CALL", instruction);
    return;
  case OpCode::StackUp:
    simple_instruction("STACK_UP", instruction);
    return;
  case OpCode::Pop:
    simple_instruction("POP", instruction);
    return;
  case OpCode::True:
    simple_instruction("TRUE", instruction);
    return;
  case OpCode::False:
    simple_instruction("FALSE", instruction);
    return;
  case OpCode::Not:
    simple_instruction("NOT", instruction);
    return;
  case OpCode::Equal:
    simple_instruction("EQUAL", instruction);
    return;
  case OpCode::Greater:
    simple_instruction("GREATER", instruction);
    return;
  case OpCode::Less:
    simple_instruction("LESS", instruction);
    return;
  case OpCode::And:
    simple_instruction("AND", instruction);
    return;
  case OpCode::Or:
    simple_instruction("OR", instruction);
    return;
  case OpCode::Log:
    simple_instruction("LOG", instruction);
    return;
  case OpCode::TypeArg:
    arg_instruction("TYPE_ARG", instruction);
    return;
  case OpCode::Add:
    simple_instruction("ADD", instruction);
    return;
  case OpCode::Multiply:
    simple_instruction("MULTIPLY", instruction);
    return;
  case OpCode::Divide:
    simple_instruction("DIVIDE", instruction);
    return;
  case OpCode::Negate:
    simple_instruction("NEGATE", instruction);
    return;
  case OpCode::Return:
    simple_instruction("RETURN", instruction);
    return;
  default:
    printf("Unknown opcode %d\n", byte);
    instruction++;
    return;
  }
}
} // namespace

void GuiSE::disassemble(const ByteCode &byte_code) {
  int offset = 0;
  const uint8_t *instruction = byte_code[offset];
  while (offset < byte_code.Length()) {
    printf("%04d ", offset);
    disassemble_instruction(instruction);
    offset = instruction - byte_code[0];
  }
}
