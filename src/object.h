#pragma once

#include <memory>
#include <string>

namespace GuiSE {
enum class ObjectType {
  String,
};

class Object {
public:
  virtual ~Object() = default;
};

class String : public Object {
public:
  String(const char *chars, int length);

  const std::string &get_str() const;

private:
  std::string _str;
};
} // namespace GuiSE