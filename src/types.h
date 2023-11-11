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

enum class ValueType : uint8_t {
  Invalid,
  Bool,
  Number,
};

struct Value {
  union {
    Bool bool_;
    Number number;
  };

  Value() : number(0) {}

  Value(Bool boolean) : bool_(boolean) {}

  Value(Number number) : number(number) {}
};

void printValue(ValueType type, Value value);
} // namespace GuiSE