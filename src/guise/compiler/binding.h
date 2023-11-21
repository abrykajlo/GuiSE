#pragma once

#include "types.h"

namespace GuiSE {
struct Binding {
  const char *identifier;
  Value default_value;
  ValueType value_t;
  bool has_default;
};

struct TypeBinding {};

struct Scope {};

struct GlobalScope : Scope {};
} // namespace GuiSE