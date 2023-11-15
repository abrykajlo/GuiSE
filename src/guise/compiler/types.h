#pragma once

#include <cstdint>
#include <cstdio>

namespace GuiSE {
// str
class Str;

// bool
using Bool = bool;

// floating
using Number = double;

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
    Str *str;
  };

  Value() : number(0) {}

  Value(Bool boolean) : bool_(boolean) {}

  Value(Number number) : number(number) {}

  Value(Str *str) : str(str) {}
};

void log_value(ValueType type, Value value);
} // namespace GuiSE