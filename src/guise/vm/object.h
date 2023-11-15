#pragma once

#include <memory>
#include <string>

namespace GuiSE {
class Str {
public:
  Str(const char *chars, int length);
  ~Str();

  const std::string &get_str() const;

private:
  std::string _str;
};
} // namespace GuiSE