#pragma once

#include <guise/compiler/types.h>

#define STACK_MAX 256
#define FRAMES_MAX 64

namespace GuiSE {
class ByteCode;
class Obj;

enum class InterpretResult { Ok, CompileError, RuntimeError };

struct CallFrame {
  const uint8_t *ip = nullptr;
  const Value *fp = nullptr;
};

struct Registers {
  const uint8_t *ip = nullptr;       // instruction pointer
  Value *fp = nullptr;               // frame pointer
  Value *sp = nullptr;               // stack pointer
  CallFrame *cf = nullptr;           // call frame
  Value va;                          // a register
  Value vb;                          // b register
  ValueType tr = ValueType::Invalid; // type register
};

class VM {
public:
  InterpretResult Run();

  InterpretResult Call(const char *function_name);

  void set_byte_code(const ByteCode &byte_code);

private:
  template <typename T> inline T _read() { return static_cast<T>(_read()); }
  inline uint8_t _read() { return *_regs.ip++; }

  void _reset();
  void _push(Value value);
  Value _pop();

  template <auto Value::*member, typename F> inline void _binary_op(F f) {
    const auto b = _pop().*member;
    const auto a = _pop().*member;
    _push(f(a, b));
  }

  Registers _regs;
  const ByteCode *_byte_code = nullptr;
  Value _stack[STACK_MAX];
  CallFrame _call_stack[FRAMES_MAX];
};
} // namespace GuiSE