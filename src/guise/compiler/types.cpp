#include "types.h"

#include <guise/vm/object.h>

void GuiSE::log_value(ValueType type, Value value) {
  switch (type) {
  case ValueType::Bool:
    printf(value.bool_ ? "true" : "false");
    break;
  case ValueType::Number:
    printf("%g", value.number);
    break;
  case ValueType::Str:
    printf("%s", value.str->get_str().c_str());
    break;
  }
}