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
  ValueType type;

  union {
    Bool bool_;
    Num num;
    Int int_;
    Str *str;
  };

  Value() : type(ValueType::Invalid), num(0) {}

  Value(Bool boolean) : type(ValueType::Bool), bool_(boolean) {}

  Value(Num num) : type(ValueType::Num), num(num) {}

  Value(Str *str) : type(ValueType::Str), str(str) {}
};

void log_value(Value value);
} // namespace GuiSE