#include "object.h"

using namespace GuiSE;

Str::Str(const char *chars, int length) : _str(chars, length) {}

GuiSE::Str::~Str() {}

const std::string &Str::get_str() const { return _str; }
