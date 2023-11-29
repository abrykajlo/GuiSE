#include "types.h"

#include <guise/vm/object.h>

void GuiSE::log_value(ValueType type, Value value) {
  switch (type) {
  case ValueType::Bool:
    printf(value.bool_ ? "true" : "false");
    break;
  case ValueType::Num:
    printf("%g", value.num);
    break;
  case ValueType::Int:
    printf("%d", value.int_);
    break;
  case ValueType::Str:
    printf("%s", value.str->get_chars());
    break;
  }
}