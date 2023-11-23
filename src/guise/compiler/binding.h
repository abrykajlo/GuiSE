#pragma once

#include "types.h"

namespace GuiSE {
struct Binding {
  const char *identifier;
  ValueType value_t;
};

struct TypeBinding {};

struct Scope {};

struct GlobalScope : Scope {};
} // namespace GuiSE