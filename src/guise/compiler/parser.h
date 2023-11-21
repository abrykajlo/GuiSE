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

class Parser {
public:
  Parser(Scanner &scanner, ByteCode &byte_code);

  bool Parse();

private:
  // parser basic management
  void _advance();
  bool _check(TokenType token_t);
  void _consume(TokenType token_t, const char *message);
  bool _match(TokenType token_t);

  // declarations
  void _declaration();
  void _binding_declaration();
  void _global_declaration();
  void _fn_declaration();
  void _type_declaration();
  void _cmpt_declaration();

  // statements
  void _statement();
  void _log_statement();

  // expressions
  void _expression();
  void _binary();
  void _grouping();
  void _literal();
  void _number();
  void _string();
  void _unary();

  // parse rules
  using ParseFn = void (Parser::*)();

  struct InfixRule {
    ParseFn rule = nullptr;
    Precedence prec = Precedence::None;
  };

  ParseFn _get_prefix_rule(TokenType token_t) const;
  const InfixRule &_get_infix_rule(TokenType token_t) const;
  void _parse_precedence(Precedence prec);

  // emitters
  void _emit_byte(uint8_t byte);

  template <typename T> inline void _emit_byte(T byte) {
    _emit_byte(static_cast<uint8_t>(byte));
  }

  void _emit_constant(Value value);
  uint8_t _make_constant(Value value);

  // errors
  void _error_at(TokenType token_t, const Token &token, const char *message);
  void _error_at_current(const char *message);
  void _error(const char *message);

  void _type_error(ValueType expected, ValueType value_t, const char *message);

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