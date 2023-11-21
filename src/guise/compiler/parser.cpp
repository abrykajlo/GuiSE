#include "parser.h"

#include <guise/debug.h>
#include <guise/vm/byte_code.h>
#include <guise/vm/object.h>
#include <guise/vm/opcode.h>

using namespace GuiSE;

namespace {
const char *expect_number_operand = "Expect number type operand.";
const char *expect_bool_operand = "Expect bool type operand.";

const char *expect_number_left_operand = "Expect number type left operand.";
const char *expect_number_right_operand = "Expect number type right operand.";

const char *expect_bool_left_operand = "Expect bool type left operand.";
const char *expect_bool_right_operand = "Expect bool type right operand.";
} // namespace

void Parser::_advance() {
  _prev_token_t = _curr_token_t;
  _prev_token = _curr_token;

  for (;;) {
    _curr_token_t = _scanner.ScanToken(_curr_token);
    if (_curr_token_t != TokenType::Error)
      break;

    _error_at_current(_curr_token.start);
  }
}

bool Parser::_check(TokenType token_t) { return _curr_token_t == token_t; }

void Parser::_consume(TokenType type, const char *message) {
  if (_curr_token_t == type) {
    _advance();
    return;
  }

  _error_at_current(message);
}

bool Parser::_match(TokenType token_t) {
  if (_check(token_t)) {
    _advance();
    return true;
  }
  return false;
}

void Parser::_type_error(ValueType expected, ValueType type,
                         const char *message) {
  if (expected != type) {
    _error(message);
  }
}

Parser::ParseFn Parser::_get_prefix_rule(TokenType token_t) const {
  switch (token_t) {
  case TokenType::True:
  case TokenType::False:
    return &Parser::_literal;
  case TokenType::Bang:
  case TokenType::Minus:
    return &Parser::_unary;
  case TokenType::Number:
    return &Parser::_number;
  case TokenType::OpenParen:
    return &Parser::_grouping;
  case TokenType::String:
    return &Parser::_string;
  }

  return nullptr;
}

const Parser::InfixRule &Parser::_get_infix_rule(TokenType token_t) const {
  static InfixRule or_rule = {&Parser::_binary, Precedence::Or};
  static InfixRule and_rule{&Parser::_binary, Precedence::And};
  static InfixRule equality_rule = {&Parser::_binary, Precedence::Equality};
  static InfixRule comparison_rule = {&Parser::_binary, Precedence::Comparison};
  static InfixRule term_rule = {&Parser::_binary, Precedence::Term};
  static InfixRule factor_rule = {&Parser::_binary, Precedence::Factor};

  switch (token_t) {
  case TokenType::Or:
    return or_rule;
  case TokenType::And:
    return and_rule;
  case TokenType::BangEqual:
  case TokenType::EqualEqual:
    return equality_rule;
  case TokenType::Greater:
  case TokenType::GreaterEqual:
  case TokenType::Less:
  case TokenType::LessEqual:
    return comparison_rule;
  case TokenType::Minus:
  case TokenType::Plus:
    return term_rule;
  case TokenType::Star:
  case TokenType::Slash:
    return factor_rule;
  }

  static InfixRule empty;
  return empty;
}

void Parser::_parse_precedence(Precedence prec) {
  _advance();
  ParseFn prefix_rule = _get_prefix_rule(_prev_token_t);
  if (prefix_rule == nullptr) {
    _error("Expect expression.");
    return;
  }

  (this->*prefix_rule)();

  const InfixRule *infix_rule = &_get_infix_rule(_curr_token_t);
  while (prec <= infix_rule->prec) {
    _advance();
    const ParseFn rule = infix_rule->rule;
    (this->*rule)();
    infix_rule = &_get_infix_rule(_curr_token_t);
  }
}

void Parser::_binary() {
  ValueType left_last_value_t = _last_value_t;

  TokenType operator_t = _prev_token_t;
  const InfixRule &rule = _get_infix_rule(operator_t);
  _parse_precedence(static_cast<Precedence>(static_cast<int>(rule.prec) + 1));
  ValueType right_last_value_t = _last_value_t;

  ValueType expected_last_value_t = ValueType::Invalid;
  ValueType return_t = ValueType::Invalid;
  switch (operator_t) {
  case TokenType::Plus:
    expected_last_value_t = ValueType::Number;
    _emit_byte(OpCode::Add);
    return_t = ValueType::Number;
    break;
  case TokenType::Minus:
    expected_last_value_t = ValueType::Number;
    _emit_byte(OpCode::Negate);
    _emit_byte(OpCode::Add);
    return_t = ValueType::Number;
    break;
  case TokenType::Star:
    expected_last_value_t = ValueType::Number;
    _emit_byte(OpCode::Multiply);
    return_t = ValueType::Number;
    break;
  case TokenType::Slash:
    expected_last_value_t = ValueType::Number;
    _emit_byte(OpCode::Divide);
    return_t = ValueType::Number;
    break;
  case TokenType::BangEqual:
    expected_last_value_t = ValueType::Number;
    _emit_byte(OpCode::Equal);
    _emit_byte(OpCode::Not);
    return_t = ValueType::Bool;
    break;
  case TokenType::EqualEqual:
    expected_last_value_t = ValueType::Number;
    _emit_byte(OpCode::Equal);
    return_t = ValueType::Bool;
    break;
  case TokenType::Greater:
    expected_last_value_t = ValueType::Number;
    _emit_byte(OpCode::Greater);
    return_t = ValueType::Bool;
    break;
  case TokenType::GreaterEqual:
    expected_last_value_t = ValueType::Number;
    _emit_byte(OpCode::Less);
    _emit_byte(OpCode::Not);
    return_t = ValueType::Bool;
    break;
  case TokenType::Less:
    expected_last_value_t = ValueType::Number;
    _emit_byte(OpCode::Less);
    return_t = ValueType::Bool;
    break;
  case TokenType::LessEqual:
    expected_last_value_t = ValueType::Number;
    _emit_byte(OpCode::Greater);
    _emit_byte(OpCode::Not);
    return_t = ValueType::Bool;
    break;
  case TokenType::Or:
    expected_last_value_t = ValueType::Bool;
    _emit_byte(OpCode::Or);
    return_t = ValueType::Bool;
    break;
  case TokenType::And:
    expected_last_value_t = ValueType::Bool;
    _emit_byte(OpCode::And);
    return_t = ValueType::Bool;
    break;
  }

  const char *expect_left_message = nullptr;
  const char *expect_right_message = nullptr;
  if (expected_last_value_t == ValueType::Bool) {
    expect_left_message = expect_bool_left_operand;
    expect_right_message = expect_bool_right_operand;
  } else if (expected_last_value_t == ValueType::Number) {
    expect_left_message = expect_number_left_operand;
    expect_right_message = expect_number_right_operand;
  }
  _type_error(expected_last_value_t, left_last_value_t, expect_left_message);
  _type_error(expected_last_value_t, right_last_value_t, expect_right_message);

  _last_value_t = return_t;
}

Parser::Parser(Scanner &scanner, ByteCode &byte_code)
    : _scanner(scanner), _byte_code(&byte_code) {
  _advance();
}

bool Parser::Parse() {
  while (!_match(TokenType::Eof)) {
    _global_declaration();
  }

  return !_had_error;
}

void Parser::_declaration() { _statement(); }

void Parser::_binding_declaration() {}

void Parser::_global_declaration() {
  if (_match(TokenType::Fn)) {
    _fn_declaration();
  } else {
    _error_at_current("Expect global declaration.");
  }
}

void Parser::_fn_declaration() {
    _match(TokenType::Identifier);
  _consume(TokenType::OpenBrace, "Expect '{'.");
  while (!_match(TokenType::CloseBrace)) {
    _declaration();
  }
}

void Parser::_type_declaration() {}

void Parser::_cmpt_declaration() {}

void Parser::_statement() {
  if (_match(TokenType::Log)) {
    _log_statement();
  }
}

void Parser::_log_statement() {
  _expression();
  _type_error(ValueType::Str, _last_value_t, "Exoect value of type str");
  _consume(TokenType::SemiColon, "Expect ';' after value.");
  _emit_byte(OpCode::Log);
}

void Parser::_expression() { _parse_precedence(Precedence::Assignment); }

void Parser::_grouping() {
  _expression();
  _consume(TokenType::CloseParen, "Expect ')' after expression.");
}

void Parser::_literal() {
  switch (_prev_token_t) {
  case TokenType::True:
    _emit_byte(OpCode::True);
    _last_value_t = ValueType::Bool;
    break;
  case TokenType::False:
    _emit_byte(OpCode::False);
    _last_value_t = ValueType::Bool;
    break;
  }
}

void Parser::_number() {
  Value value = strtod(_prev_token.start, nullptr);
  _emit_constant(value);

  _last_value_t = ValueType::Number;
}

void Parser::_string() {
  _emit_constant(new Str(_prev_token.start + 1, _prev_token.length - 2));

  _last_value_t = ValueType::Str;
}

void Parser::_unary() {
  TokenType operator_t = _prev_token_t;

  _parse_precedence(Precedence::Unary);
  ValueType value_t = _last_value_t;

  switch (operator_t) {
  case TokenType::Minus:
    _type_error(ValueType::Number, value_t, "Expect number type operand.");
    _emit_byte(OpCode::Negate);
    break;
  case TokenType::Bang:
    _type_error(ValueType::Bool, value_t, "Expect bool type operand.");
    _emit_byte(OpCode::Not);
    break;
  }
}

void Parser::_emit_byte(uint8_t byte) {
  GUISE_ASSERT(_byte_code != nullptr)

  _byte_code->Write(byte);
}

void Parser::_emit_constant(Value value) {
  _emit_byte(OpCode::Constant);
  _emit_byte(_make_constant(value));
}

uint8_t Parser::_make_constant(Value value) {
  GUISE_ASSERT(_byte_code != nullptr)

  int constant = _byte_code->AddConstant(value);
  if (constant > std::numeric_limits<uint8_t>::max()) {
    _error("Too many constants in one chunk.");
    return 0;
  }

  return static_cast<uint8_t>(constant);
}

void Parser::_error_at(TokenType token_t, const Token &token,
                       const char *message) {
  fprintf(stderr, "[line %d] Error", token.line);

  if (token_t == TokenType::Eof) {
    fprintf(stderr, " at end");
  } else if (token_t == TokenType::Error) {
    // Nothing.
  } else {
    fprintf(stderr, " at '%.*s'", token.length, token.start);
  }

  fprintf(stderr, ": %s\n", message);
  _had_error = true;
}

void Parser::_error_at_current(const char *message) {
  _error_at(_curr_token_t, _curr_token, message);
}

void Parser::_error(const char *message) {
  _error_at(_prev_token_t, _prev_token, message);
}
