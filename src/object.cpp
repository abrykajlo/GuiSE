#include "object.h"

using namespace GuiSE;

String::String(const char *chars, int length) : _str(chars, length) {}

const std::string &GuiSE::String::get_str() const { return _str; }
