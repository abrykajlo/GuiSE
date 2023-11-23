#pragma once

#include <memory>
#include <string>

namespace GuiSE {
class Obj {
public:
  Obj();
  virtual ~Obj();

  inline Obj *get_next() { return _next; }
  inline void set_next(Obj *obj) { _next = obj; }

private:
  Obj *_next = nullptr;
};

class Str : Obj {
public:
  Str(const char *chars, int length);
  virtual ~Str();

  inline char *get_chars() const { return _chars; }

private:
  int _length;
  char *_chars;
};
} // namespace GuiSE