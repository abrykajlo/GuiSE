#include "object.h"

using namespace GuiSE;

Obj::Obj() {}

Obj::~Obj() {
  if (_next != nullptr)
    delete _next;
}

Str::Str(const char *chars, int length)
    : _length(length), _chars(new char[length + 1]) {
  memcpy(_chars, chars, length);
  _chars[_length] = '\0';
}

Str::~Str() {
  if (_chars != nullptr)
    delete[] _chars;
}
