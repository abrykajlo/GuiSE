#pragma once

#include "types.h"

#include <vector>

namespace GuiSE {
using Value = f64;

using ValueArray = std::vector<Value>;

void printValue(Value value);
} // namespace GuiSE