#pragma once

#include <cstdint>

namespace GuiSE {
// str
using Str = const char *;

// bool
using Bool = bool;

// floating
using Number = double;

struct Value {
  union {
    Bool boolean;
    Number number;
  };

  Value() : number(0) {}

  Value(Bool boolean) : boolean(boolean) {}

  Value(Number number) : number(number) {}
};
} // namespace GuiSE