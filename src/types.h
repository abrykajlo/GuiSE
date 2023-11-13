#pragma once

#include <cstdint>
#include <cstdio>

namespace GuiSE {
// str
using Str = const char *;

// bool
using Bool = bool;

// floating
using Number = double;

// objects
class Object;

enum class ValueType : uint8_t {
  Invalid,
  Bool,
  Number,
  Str,
};

struct Value {
  union {
    Bool bool_;
    Number number;
    Object *object;
  };

  Value() : number(0) {}

  Value(Bool boolean) : bool_(boolean) {}

  Value(Number number) : number(number) {}

  Value(Object *object) : object(object) {}
};

void log_value(ValueType type, Value value);
} // namespace GuiSE