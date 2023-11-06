#include "vm.h"

#include "compiler.h"
#include "opcode.h"

#define _RUN_CASE(OPCODE)                                                      \
  case OpCode::OPCODE:                                                         \
    _read<OpCode::OPCODE>();

#define _BINARY_OP(OP)                                                         \
  {                                                                            \
    Number b = _pop();                                                         \
    Number a = _pop();                                                         \
    _push(a OP b);                                                             \
  }

using namespace GuiSE;

InterpretResult VM::Interpret(const ByteCode &byte_code) {
  _byte_code = &byte_code;
  _ip = _byte_code->GetByteCodePtr();
  return _run();
}

InterpretResult VM::Interpret(const char *source) {
  ByteCode byte_code;

  if (!compile(source, byte_code)) {
    return InterpretResult::CompileError;
  }

  return Interpret(byte_code);
}

InterpretResult VM::_run() {
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
        _BINARY_OP(+)
        break;
      }
      _RUN_CASE(Multiply) {
        _BINARY_OP(*)
        break;
      }
      _RUN_CASE(Divide) {
        _BINARY_OP(/)
        break;
      }
      _RUN_CASE(Return) { return InterpretResult::Ok; }
      _RUN_CASE(NoOp) { return InterpretResult::Ok; }
    }
  }
  return InterpretResult();
}

void VM::_reset_stack() { _stack_top = _stack; }

void VM::_push(Number value) {
  *_stack_top = value;
  _stack_top++;
}

Number VM::_pop() {
  _stack_top--;
  return *_stack_top;
}
