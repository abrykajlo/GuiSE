#include "types.h"

#include <guise/vm/object.h>

const char *GuiSE::type_string(ValueType type) {
  switch (type) {
  case ValueType::Invalid:
    return "invalid";
  case ValueType::Bool:
    return "bool";
  case ValueType::Num:
    return "num";
  case ValueType::Int:
    return "int";
  case ValueType::Str:
    return "str";
  case ValueType::Void:
    return "void";
  }
}

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