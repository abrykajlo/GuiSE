#include "types.h"

void GuiSE::printValue(ValueType type, Value value) {
  switch (type) {
  case ValueType::Bool:
    printf(value.bool_ ? "true" : "false");
    break;
  case ValueType::Number:
    printf("%g", value.number);
    break;
  }
}