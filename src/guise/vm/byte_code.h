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

  void AddFunction(const std::string &function_name);
  const uint8_t *GetFunction(const std::string &function_name) const;

  int AddConstant(Value value);
  Value GetConstant(int index) const;

  const std::vector<uint8_t> &get_byte_code() const;

private:
  std::vector<uint8_t> _byte_code;
  std::vector<Value> _constants;
  std::map<std::string, int> _function_offsets;
};
} // namespace GuiSE