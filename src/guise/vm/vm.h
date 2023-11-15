#pragma once

#include <guise/compiler/types.h>

#define STACK_MAX 256

namespace GuiSE {
class ByteCode;

enum class InterpretResult { Ok, CompileError, RuntimeError };

class VM {
public:
  InterpretResult Interpret(const ByteCode &byte_code);
  InterpretResult Interpret(const char *source);

private:
  InterpretResult _run();

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