#pragma once

#include <cstdint>
#include <cstdio>

namespace GuiSE {
// str
class Str;

// bool
using Bool = bool;

// floating
using Num = double;

// integer
using Int = int8_t;

enum class ValueType : uint8_t {
  Invalid,
  Bool,
  Num,
  Int,
  Str,
  Void,
};

struct Value {
  union {
    Bool bool_;
    Num num;
    Int int_;
    Str *str;
  };

  Value() : bool_(false) {}

  Value(Bool bool_) : bool_(bool_) {}

  Value(Num num) : num(num) {}

  Value(Int int_) : int_(int_) {}

  Value(Str *str) : str(str) {}
};

void log_value(ValueType type, Value value);
} // namespace GuiSE