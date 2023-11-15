#include "vm.h"

#include "byte_code.h"
#include "opcode.h"

#include <guise/compiler/compiler.h>

using namespace GuiSE;

namespace {
// basic arithmetic operator
inline Number op_plus(Number a, Number b) { return a + b; }

inline Number op_multiply(Number a, Number b) { return a * b; }

inline Number op_divide(Number a, Number b) { return a / b; }

// comparison operators
inline Bool op_equal(Number a, Number b) { return a == b; }

inline Bool op_greater(Number a, Number b) { return a > b; }

inline Bool op_less(Number a, Number b) { return a < b; }

// boolean operations
inline Bool op_or(Bool a, Bool b) { return a || b; }

inline Bool op_and(Bool a, Bool b) { return a && b; }
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
    } break;
    case OpCode::Add:
      _binary_op<&Value::number>(op_plus);
      break;
    case OpCode::Negate:
      _push(-_pop().number);
      break;
    case OpCode::Multiply:
      _binary_op<&Value::number>(op_multiply);
      break;
    case OpCode::Divide:
      _binary_op<&Value::number>(op_divide);
      break;
    case OpCode::True:
      _push(true);
      break;
    case OpCode::False:
      _push(false);
      break;
    case OpCode::Not:
      _push(!_pop().bool_);
      break;
    case OpCode::Equal:
      _binary_op<&Value::number>(op_equal);
      break;
    case OpCode::Greater:
      _binary_op<&Value::number>(op_greater);
      break;
    case OpCode::Less:
      _binary_op<&Value::number>(op_less);
      break;
    case OpCode::And:
      _binary_op<&Value::bool_>(op_and);
      break;
    case OpCode::Or:
      _binary_op<&Value::bool_>(op_or);
      break;
    case OpCode::Log: {
      ValueType type = _read<ValueType>();
      log_value(type, _pop());
    } break;
    case OpCode::Return: {
      return InterpretResult::Ok;
    }
    case OpCode::NoOp: {
      return InterpretResult::Ok;
    }
    }
  }

  return InterpretResult::Ok;
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
