#include "vm.h"

#include "opcode.h"

#define _RUN_CASE(OPCODE)                                                      \
  case OpCode::OPCODE:                                                         \
    _read<OpCode::OPCODE>();

using namespace GuiSE;

InterpretResult VM::Interpret(ByteCode *ByteCode) {
  _byte_code = ByteCode;
  _ip = _byte_code->GetByteCodePtr();
  return Run();
}

InterpretResult VM::Run() {
  _reset_stack();
  for (;;) {
    auto instruction = _peek();
    switch (instruction->op_code) {
      _RUN_CASE(Push) {
        _push(instruction->op_push.value);
        break;
      }
      _RUN_CASE(Negate) {
        _push(-_pop());
        break;
      }
      _RUN_CASE(Add) {
        auto b = _pop();
        auto a = _pop();
        _push(a + b);
        break;
      }
      _RUN_CASE(Return) { return InterpretResult::Ok; }
      _RUN_CASE(NoOp) { return InterpretResult::Ok; }
    }
  }
  return InterpretResult();
}

void VM::_reset_stack() { _stack_top = _stack; }

void VM::_push(f64 value) {
  *_stack_top = value;
  _stack_top++;
}

f64 VM::_pop() {
  _stack_top--;
  return *_stack_top;
}
