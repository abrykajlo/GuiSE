#pragma once

#include "scanner.h"
#include "types.h"

namespace GuiSE {
class ByteCode;
class Scanner;

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

class Parser;

using ParseFn = void (Parser::*)();

struct ParseRule {
  ParseFn prefix = nullptr;
  ParseFn infix = nullptr;
  Precedence prec = Precedence::None;
};

class Parser {
public:
  Parser(Scanner &scanner, ByteCode &byte_code);

  bool IsDone() const;

  void Expression();
  void Binary();
  void Grouping();
  void Literal();
  void Number();
  void String();
  void Unary();

  inline bool get_had_error() { return _had_error; }

private:
  void _advance();
  void _consume(TokenType token_t, const char *message);
  void _type_error(ValueType expected, ValueType value_t, const char *message);

  const ParseRule &_get_parse_rule(TokenType token_t) const;
  void _parse_precedence(Precedence prec);

  void _emit_byte(uint8_t byte);

  template <typename T> inline void _emit_byte(T byte) {
    _emit_byte(static_cast<uint8_t>(byte));
  }

  void _emit_constant(Value value);
  uint8_t _make_constant(Value value);

  void _error_at(TokenType token_t, const Token &token, const char *message);
  void _error_at_current(const char *message);
  void _error(const char *message);

  Scanner &_scanner;
  ByteCode *_byte_code;
  Token _curr_token;
  Token _prev_token;
  TokenType _curr_token_t = TokenType::Invalid;
  TokenType _prev_token_t = TokenType::Invalid;
  ValueType _last_value_t = ValueType::Invalid;
  bool _had_error = false;
  bool _panic_mode = false;
};
} // namespace GuiSE