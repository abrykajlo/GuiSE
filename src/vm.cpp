#include "vm.h"

#include "byte_code.h"
#include "compiler.h"
#include "opcode.h"

using namespace GuiSE;

namespace {
inline Number plus(Number a, Number b) { return a + b; }

inline Number multiply(Number a, Number b) { return a * b; }

inline Number divide(Number a, Number b) { return a / b; }
} // namespace

InterpretResult VM::Interpret(const ByteCode &byte_code) {
  _byte_code = &byte_code;
  _ip = &_byte_code->get_byte_code()[0];
  return _run();
}

InterpretResult VM::Interpret(const char *source) {
  Compiler compiler;
  ByteCode byte_code;

  if (!compiler.Compile(source, byte_code)) {
    return InterpretResult::CompileError;
  }

  return Interpret(byte_code);
}

InterpretResult VM::_run() {
  _reset_stack();
  for (;;) {
    OpCode op_code;
    switch (op_code = _read<OpCode>()) {
    case OpCode::Constant: {
      const Value value = _byte_code->GetConstant(_read());
      _push(value);
      break;
    }
    case OpCode::Negate: {
      _push(-_pop().number);
      break;
    }
    case OpCode::Add: {
      _binary_op<&Value::number>(plus);
      break;
    }
    case OpCode::Multiply: {
      _binary_op<&Value::number>(multiply);
      break;
    }
    case OpCode::Divide: {
      _binary_op<&Value::number>(divide);
      break;
    }
    case OpCode::Log: {
      ValueType type = _read<ValueType>();
      printValue(type, _pop());
      break;
    }
    case OpCode::Return: {
      return InterpretResult::Ok;
    }
    case OpCode::NoOp: {
      return InterpretResult::Ok;
    }
    }
  }
  return InterpretResult();
}

void VM::_reset_stack() { _stack_top = _stack; }

void VM::_push(Value value) {
  *_stack_top = value;
  _stack_top++;
}

Value VM::_pop() {
  _stack_top--;
  return *_stack_top;
}
