#pragma once

#include <cstdint>
#include <map>
#include <string>
#include <vector>

namespace GuiSE {
struct Value;
class ByteCode {
public:
  void Write(uint8_t byte);

  void AddFunction(const std::string &function_name, int fn_ptr);
  const uint8_t *GetFunction(const std::string &function_name) const;

  int AddConstant(Value value);
  Value GetConstant(int index) const;

  const uint8_t *operator[](const size_t i) const;

  size_t Length() const;

private:
  std::vector<uint8_t> _byte_code;
  std::vector<Value> _constants;
  std::map<std::string, int> _functions;
  int _global_count = 0;
};
} // namespace GuiSE