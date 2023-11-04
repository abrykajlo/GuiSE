#include "byte_code.h"

#include "opcode.h"

namespace {
int simple_instruction(const char *name, int offset) {
  printf("%s\n", name);
  return offset + 1;
}
} // namespace

using namespace GuiSE;

const uint8_t *ByteCode::GetByteCodePtr() const {
  return _byte_code.empty() ? nullptr : _byte_code.data();
}

void ByteCode::Disassemble(const char *name) const {
  printf("== %s ==\n", name);

  for (int offset = 0; offset < _byte_code.size();) {
    offset = _disassemble_instruction(offset);
  }
}

int ByteCode::_disassemble_instruction(int offset) const {
  printf("%04d ", offset);

  auto instruction_set =
      reinterpret_cast<const InstructionSet *>(_byte_code.data() + offset);
  switch (auto op_code = instruction_set->op_code) {
  case OpCode::Push:
    return _push_instruction("PUSH", instruction_set->op_push, offset);
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
    printf("Unknown opcode %d\n", static_cast<int>(op_code));
    return offset + 1;
  }
}

int ByteCode::_push_instruction(const char *name,
                                const Instruction<OpCode::Push> &instruction,
                                int offset) const {
  printf("%-16s %g\n", name, instruction.value);
  return offset + sizeof(instruction);
}
