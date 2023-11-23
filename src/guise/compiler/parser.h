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
  void _identifier(std::string &identifier);
  void _type_specifier(ValueType &value_t);
  void _declaration();
  void _binding_declaration();
  void _global_declaration();
  void _fn_declaration(const std::string &identifier);
  void _type_declaration();
  void _cmpt_declaration();

  // statements
  void _statement();
  void _log_statement();

  // expressions
  ValueType _binary(ValueType left_t);
  ValueType _expression();
  ValueType _grouping();
  ValueType _literal();
  ValueType _number();
  ValueType _string();
  ValueType _unary();

  // parse rules
  using ParsePrefixFn = ValueType (Parser::*)();
  using ParseInfixFn = ValueType (Parser::*)(ValueType);

  struct InfixRule {
    ParseInfixFn rule = nullptr;
    Precedence prec = Precedence::None;
  };

  ParsePrefixFn _get_prefix_rule(TokenType token_t) const;
  const InfixRule &_get_infix_rule(TokenType token_t) const;
  ValueType _parse_precedence(Precedence prec);

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
  bool _had_error = false;
  bool _panic_mode = false;
};
} // namespace GuiSE