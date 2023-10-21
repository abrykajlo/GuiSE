#pragma once

#include "types.h"

#include <cstdint>
#include <functional>

#define _OPERANDS(OPCODE, ...)                                                 \
  template <> struct Operands<OpCode::OPCODE, __VA_ARGS__> : std::true_type {};

#define _MATCH_OPS(OPCODE, ...)                                                \
  typename = std::enable_if_t<Operands<OPCODE, __VA_ARGS__>::value>

namespace GuiSE {
enum class OpCode : u8 {
  NOOP,
  LINE,
  CONSTANT,
  NEGATE,
  RETURN,
};

namespace internal {
template <OpCode I, typename... T> struct Operands : std::false_type {};

_OPERANDS(NOOP)
_OPERANDS(RETURN)
_OPERANDS(NEGATE)
_OPERANDS(LINE, u8)
_OPERANDS(CONSTANT, u8)
} // namespace internal

using namespace internal;

template <typename Reader> class OpCodeReader {
public:
  OpCodeReader(Reader &reader) : _reader(reader) {}

  OpCode read() { return static_cast<OpCode>(_read()); }

  template <OpCode I, _MATCH_OPS(I, u8)> void read(u8 &arg1) { arg1 = _read(); }

private:
  u8 _read() { return _reader.read(); }

  Reader &_reader;
};

template <typename Writer> class OpCodeWriter {
public:
  OpCodeWriter(Writer &writer) : _writer(writer) {}

  template <OpCode I, _MATCH_OPS(I)> void write() { _write<I>(); }

  template <OpCode I, _MATCH_OPS(I, u8)> void write(u8 byte) {
    _write<I>();
    _write(byte);
  }

private:
  template <OpCode I> inline void _write() {
    _writer.write(static_cast<u8>(I));
  }

  inline void _write(u8 byte) { _writer.write(byte); }

  Writer &_writer;
};
} // namespace GuiSE

#undef _MATCH_OPS
#undef _OPERANDS
