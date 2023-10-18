#include "vm.h"

#include "opcode.h"

using namespace GuiSE;

InterpretResult VM::interpret(Chunk *chunk) {
  mChunk = chunk;
  mIP = mChunk->getCodePtr();
  return run();
}

InterpretResult VM::run() {
  for (;;) {
    OpCode instruction;
    switch (instruction = readByte<OpCode>()) {
    case OpCode::RETURN:
      return InterpretResult::OK;
    case OpCode::CONSTANT:
      Value constant = readConstant();
      printValue(constant);
      printf("\n");
      break;
    }
  }
  return InterpretResult();
}
