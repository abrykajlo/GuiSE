#pragma once

#include <cstdint>
#include <vector>

namespace GuiSE {
struct Value;
class ByteCode {
public:
  void Write(uint8_t byte);

  int AddConstant(Value value);
  Value GetConstant(int index) const;

  const std::vector<uint8_t> &get_byte_code() const;

private:
  std::vector<uint8_t> _byte_code;
  std::vector<Value> _constants;
};
} // namespace GuiSE