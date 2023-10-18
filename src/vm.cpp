#include "vm.h"

#include "opcode.h"

using namespace GuiSE;

InterpretResult VM::interpret(Chunk *chunk) {
  _chunk = chunk;
  _ip = _chunk->getCodePtr();
  return run();
}

InterpretResult VM::run() {
  OpCodeReader opcodeReader(*this);
  for (;;) {
    OpCode instruction;
    switch (instruction = opcodeReader.read()) {
    case OpCode::RETURN:
      return InterpretResult::OK;
    case OpCode::CONSTANT:
      u8 constantId;
      opcodeReader.read<OpCode::CONSTANT>(constantId);
      Value constant = _chunk->getConstant(constantId);
      printValue(constant);
      printf("\n");
      break;
    }
  }
  return InterpretResult();
}
