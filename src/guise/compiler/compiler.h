#pragma once

#include "scanner.h"
#include "types.h"

#include <guise/vm/opcode.h>

namespace GuiSE {
class ByteCode;

enum class Precedence {
  None,
  Assignment,
  Or,
  And,
  Equality,
  Comparison,
  Term,
  Factor,
  Unary,
  Call,
  Primary,
};

class Compiler;

using ParseFn = void (Compiler::*)();

struct ParseRule {
  ParseFn prefix = nullptr;
  ParseFn infix = nullptr;
  Precedence prec = Precedence::None;
};

struct Parser {
  Token curr;
  Token prev;
  ValueType value_type = ValueType::Invalid;
  bool had_error = false;
  bool panic_mode = false;
};

class Compiler {
public:
  bool Compile(const char *source, ByteCode &byte_code);

private:
  void _advance();
  void _consume(TokenType type, const char *message);
  void _type_error(ValueType expected, ValueType type, const char *message);

  const ParseRule &_get_parse_rule(TokenType token_type) const;
  void _parse_precedence(Precedence prec);

  void _expression();

  void _binary();
  void _grouping();
  void _literal();
  void _number();
  void _string();
  void _unary();

  void _emit_byte(uint8_t byte);

  template <typename T> inline void _emit_byte(T byte) {
    _emit_byte(static_cast<uint8_t>(byte));
  }

  void _emit_constant(Value value);
  uint8_t _make_constant(Value value);

  void _error_at(const Token &token, const char *message);
  void _error_at_current(const char *message);
  void _error(const char *message);

  template <TokenType T> static inline ParseRule _parse_rule;
  static ParseRule _parse_rules[];

  Parser _parser;
  Scanner _scanner;
  ByteCode *_byte_code = nullptr;
};
} // namespace GuiSE