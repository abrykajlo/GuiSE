#pragma once

#include <guise/compiler/types.h>

#define STACK_MAX 256

namespace GuiSE {
class ByteCode;
class Obj;

enum class InterpretResult { Ok, CompileError, RuntimeError };

class VM {
public:
  InterpretResult Run();

  InterpretResult Call(const char *function_name);

  void set_byte_code(const ByteCode &byte_code);

private:
  template <typename T> inline T _read() { return static_cast<T>(_read()); }
  inline uint8_t _read() { return *_ip++; }

  void _reset_stack();
  void _push(Value value);
  Value _pop();

  template <auto Value::*member, typename F> inline void _binary_op(F f) {
    const auto b = _pop().*member;
    const auto a = _pop().*member;
    _push(f(a, b));
  }

  const ByteCode *_byte_code = nullptr;
  const uint8_t *_ip = nullptr;
  Value _stack[STACK_MAX];
  Value *_stack_top = _stack;
};
} // namespace GuiSE