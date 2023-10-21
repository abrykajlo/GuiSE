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
    case OpCode::CONSTANT:
      u8 constantId;
      opcodeReader.read<OpCode::CONSTANT>(constantId);
      const Value constant = _chunk->getConstant(constantId);
      printValue(constant);
      printf("\n");
      break;
    case OpCode::NEGATE:
      push(-pop());
      break;
    case OpCode::RETURN:
      return InterpretResult::OK;
    }
  }
  return InterpretResult();
}

void VM::resetStack() { _stackTop = _stack; }

void GuiSE::VM::push(Value value) {
  *_stackTop = value;
  _stackTop++;
}

Value GuiSE::VM::pop() {
  _stackTop--;
  return *_stackTop;
}
