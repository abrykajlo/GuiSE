#include "vm.h"

#include "byte_code.h"
#include "opcode.h"

#include <guise/debug.h>

using namespace GuiSE;

namespace {
// basic arithmetic operator
inline Num op_plus(Num a, Num b) { return a + b; }

inline Num op_multiply(Num a, Num b) { return a * b; }

inline Num op_divide(Num a, Num b) { return a / b; }

// comparison operators
inline Bool op_equal(Num a, Num b) { return a == b; }

inline Bool op_greater(Num a, Num b) { return a > b; }

inline Bool op_less(Num a, Num b) { return a < b; }

// boolean operations
inline Bool op_or(Bool a, Bool b) { return a || b; }

inline Bool op_and(Bool a, Bool b) { return a && b; }
} // namespace

VM::VM() {
  _regs.cf = _call_stack;
  _regs.sp = _stack;
}

InterpretResult VM::Call(const char *function_name) {
  const uint8_t *ip = _byte_code->GetFunction(function_name);
  if (ip == nullptr) {
    printf("Function %s does not exist.", function_name);
    return InterpretResult::RuntimeError;
  }

  _regs.cf++;
  _regs.cf->fp = _regs.sp;
  _regs.cf->ip = ip;

  InterpretResult result = Run();

  return result;
}

void VM::set_byte_code(const ByteCode &byte_code) { _byte_code = &byte_code; }

InterpretResult VM::Run() {
  for (;;) {
    OpCode op_code;
    switch (op_code = _read<OpCode>()) {
    case OpCode::Constant: {
      const Value value = _byte_code->GetConstant(_read());
      _push(value);
    } break;
    case OpCode::GetGlobal:
      _push(_stack[_read()]);
      break;
    case OpCode::GetLocal:
      _push(_regs.cf->fp[_read()]);
      break;
    case OpCode::SetGlobal:
        _stack[_read()] = _pop();
        break;
    case OpCode::SetLocal:
        _regs.cf->fp[_read()] = _pop();
        break;
    case OpCode::Pop:
        _pop();
        break;
    case OpCode::Add:
      _binary_op<&Value::num>(op_plus);
      break;
    case OpCode::Negate:
      _push(-_pop().num);
      break;
    case OpCode::Multiply:
      _binary_op<&Value::num>(op_multiply);
      break;
    case OpCode::Divide:
      _binary_op<&Value::num>(op_divide);
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
      _binary_op<&Value::num>(op_equal);
      break;
    case OpCode::Greater:
      _binary_op<&Value::num>(op_greater);
      break;
    case OpCode::Less:
      _binary_op<&Value::num>(op_less);
      break;
    case OpCode::And:
      _binary_op<&Value::bool_>(op_and);
      break;
    case OpCode::Or:
      _binary_op<&Value::bool_>(op_or);
      break;
    case OpCode::Log:
      log_value(_regs.tr, _pop());
      break;
    case OpCode::TypeArg:
      _regs.tr = _read<ValueType>();
      break;
    case OpCode::Return:
      return InterpretResult::Ok;
    case OpCode::Global:
      return InterpretResult::Ok;
    case OpCode::NoOp:
      return InterpretResult::Ok;
#ifdef GUISE_DEBUG
    default:
      printf("opcode %d not implemented.", op_code);
      return InterpretResult::RuntimeError;
#endif
    }
  }
}

InterpretResult VM::RunGlobal() {
  _regs.cf->ip = (*_byte_code)[0];
  OpCode last_op_code = OpCode::Global;
  while (last_op_code != OpCode::NoOp) {
    const OpCode op_code = _read<OpCode>();
    switch (op_code) {
    case OpCode::Global:
      Run();
      break;
    case OpCode::Constant:
    case OpCode::TypeArg:
    case OpCode::GetGlobal:
    case OpCode::GetLocal:
      _read();
      break;
    }
    last_op_code = static_cast<OpCode>(*(_regs.cf->ip - 1));
  }
  return InterpretResult();
}

void VM::_push(Value value) {
  *_regs.sp = value;
  _regs.sp++;
}

Value VM::_pop() {
  _regs.sp--;
  return *_regs.sp;
}
