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

Parser::ParsePrefixFn Parser::_get_prefix_rule(TokenType token_t) const {
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

ValueType Parser::_parse_precedence(Precedence prec) {
  _advance();
  ParsePrefixFn prefix_rule = _get_prefix_rule(_prev_token_t);
  if (prefix_rule == nullptr) {
    _error("Expect expression.");
    return ValueType::Invalid;
  }

  ValueType value_t = (this->*prefix_rule)();

  const InfixRule *infix_rule = &_get_infix_rule(_curr_token_t);
  while (prec <= infix_rule->prec) {
    _advance();
    const ParseInfixFn rule = infix_rule->rule;
    value_t = (this->*rule)(value_t);
    infix_rule = &_get_infix_rule(_curr_token_t);
  }

  return value_t;
}

ValueType Parser::_binary(ValueType left_t) {
  TokenType operator_t = _prev_token_t;
  const InfixRule &rule = _get_infix_rule(operator_t);
  ValueType right_t = _parse_precedence(
      static_cast<Precedence>(static_cast<int>(rule.prec) + 1));

  ValueType expected_t = ValueType::Invalid;
  ValueType return_t = ValueType::Invalid;
  switch (operator_t) {
  case TokenType::Plus:
    expected_t = ValueType::Num;
    _emit_byte(OpCode::Add);
    return_t = ValueType::Num;
    break;
  case TokenType::Minus:
    expected_t = ValueType::Num;
    _emit_byte(OpCode::Negate);
    _emit_byte(OpCode::Add);
    return_t = ValueType::Num;
    break;
  case TokenType::Star:
    expected_t = ValueType::Num;
    _emit_byte(OpCode::Multiply);
    return_t = ValueType::Num;
    break;
  case TokenType::Slash:
    expected_t = ValueType::Num;
    _emit_byte(OpCode::Divide);
    return_t = ValueType::Num;
    break;
  case TokenType::BangEqual:
    expected_t = ValueType::Num;
    _emit_byte(OpCode::Equal);
    _emit_byte(OpCode::Not);
    return_t = ValueType::Bool;
    break;
  case TokenType::EqualEqual:
    expected_t = ValueType::Num;
    _emit_byte(OpCode::Equal);
    return_t = ValueType::Bool;
    break;
  case TokenType::Greater:
    expected_t = ValueType::Num;
    _emit_byte(OpCode::Greater);
    return_t = ValueType::Bool;
    break;
  case TokenType::GreaterEqual:
    expected_t = ValueType::Num;
    _emit_byte(OpCode::Less);
    _emit_byte(OpCode::Not);
    return_t = ValueType::Bool;
    break;
  case TokenType::Less:
    expected_t = ValueType::Num;
    _emit_byte(OpCode::Less);
    return_t = ValueType::Bool;
    break;
  case TokenType::LessEqual:
    expected_t = ValueType::Num;
    _emit_byte(OpCode::Greater);
    _emit_byte(OpCode::Not);
    return_t = ValueType::Bool;
    break;
  case TokenType::Or:
    expected_t = ValueType::Bool;
    _emit_byte(OpCode::Or);
    return_t = ValueType::Bool;
    break;
  case TokenType::And:
    expected_t = ValueType::Bool;
    _emit_byte(OpCode::And);
    return_t = ValueType::Bool;
    break;
  }

  const char *expect_left_message = nullptr;
  const char *expect_right_message = nullptr;
  if (expected_t == ValueType::Bool) {
    expect_left_message = expect_bool_left_operand;
    expect_right_message = expect_bool_right_operand;
  } else if (expected_t == ValueType::Num) {
    expect_left_message = expect_number_left_operand;
    expect_right_message = expect_number_right_operand;
  }
  _type_error(expected_t, left_t, expect_left_message);
  _type_error(expected_t, right_t, expect_right_message);

  return return_t;
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

void GuiSE::Parser::_identifier(std::string &identifier) {
  if (_match(TokenType::Identifier)) {
    identifier = std::string(_prev_token.start, _prev_token.length);
  } else {
    _error_at_current("Expect identifier.");
  }
}

void GuiSE::Parser::_type_specifier(ValueType &value_t) {
  switch (_curr_token_t) {
  case TokenType::TypeBool:
    value_t = ValueType::Bool;
    _advance();
    break;
  case TokenType::TypeInt:
    value_t = ValueType::Int;
    _advance();
    break;
  case TokenType::TypeNum:
    value_t = ValueType::Num;
    _advance();
    break;
  case TokenType::TypeStr:
    value_t = ValueType::Str;
    _advance();
    break;
  default:
    value_t = ValueType::Void;
  }
}

void Parser::_declaration() { _statement(); }

void Parser::_binding_declaration() {}

void Parser::_global_declaration() {
  std::string identifier;
  _identifier(identifier);
  _consume(TokenType::Colon, "Expect ':'.");
  if (_match(TokenType::Fn)) {
    _fn_declaration(identifier);
  } else {
    _error_at_current("Expect global declaration.");
  }
}

void Parser::_fn_declaration(const std::string &identifier) {
  ValueType return_type;
  _type_specifier(return_type);
  _byte_code->AddFunction(identifier);
  _consume(TokenType::OpenBrace, "Expect '{'.");
  while (!_match(TokenType::CloseBrace)) {
    _declaration();
  }
  _emit_byte(OpCode::Return);
}

void Parser::_type_declaration() {}

void Parser::_cmpt_declaration() {}

void Parser::_statement() {
  if (_match(TokenType::Log)) {
    _log_statement();
  }
}

void Parser::_log_statement() {
  ValueType print_value_t = _expression();
  _type_error(ValueType::Str, print_value_t, "Expect value of type str");
  _consume(TokenType::SemiColon, "Expect ';' after value.");
  _emit_byte(OpCode::Log);
}

ValueType Parser::_expression() {
  return _parse_precedence(Precedence::Assignment);
}

ValueType Parser::_grouping() {
  ValueType value_t = _expression();
  _consume(TokenType::CloseParen, "Expect ')' after expression.");
  return value_t;
}

ValueType Parser::_literal() {
  switch (_prev_token_t) {
  case TokenType::True:
    _emit_byte(OpCode::True);
    return ValueType::Bool;
  case TokenType::False:
    _emit_byte(OpCode::False);
    return ValueType::Bool;
  }
}

ValueType Parser::_number() {
  Value value = strtod(_prev_token.start, nullptr);
  _emit_constant(value);

  return ValueType::Num;
}

ValueType Parser::_string() {
  _emit_constant(new Str(_prev_token.start + 1, _prev_token.length - 2));

  return ValueType::Str;
}

ValueType Parser::_unary() {
  TokenType operator_t = _prev_token_t;
  ValueType value_t = _parse_precedence(Precedence::Unary);

  switch (operator_t) {
  case TokenType::Minus:
    _type_error(ValueType::Num, value_t, "Expect number type operand.");
    _emit_byte(OpCode::Negate);
    return ValueType::Num;
  case TokenType::Bang:
    _type_error(ValueType::Bool, value_t, "Expect bool type operand.");
    _emit_byte(OpCode::Not);
    return ValueType::Bool;
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
