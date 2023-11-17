#include "parser.h"

#include <guise/debug.h>
#include <guise/vm/byte_code.h>
#include <guise/vm/object.h>
#include <guise/vm/opcode.h>

#define DEFINE_TOKEN_RULE(TOKEN, ...)                                          \
  template <> ParseRule parse_rule<TokenType::TOKEN> = {__VA_ARGS__};

#define TOKEN_RULE(INDEX) parse_rule<static_cast<TokenType>(INDEX)>

using namespace GuiSE;

namespace {
const char *expect_number_operand = "Expect number type operand.";
const char *expect_bool_operand = "Expect bool type operand.";

const char *expect_number_left_operand = "Expect number type left operand.";
const char *expect_number_right_operand = "Expect number type right operand.";

const char *expect_bool_left_operand = "Expect bool type left operand.";
const char *expect_bool_right_operand = "Expect bool type right operand.";

template <TokenType T> static inline ParseRule parse_rule;

DEFINE_TOKEN_RULE(True, &Parser::Literal)
DEFINE_TOKEN_RULE(False, &Parser::Literal)
DEFINE_TOKEN_RULE(Number, &Parser::Number)
DEFINE_TOKEN_RULE(OpenParen, &Parser::Grouping)
DEFINE_TOKEN_RULE(String, &Parser::String)
DEFINE_TOKEN_RULE(Or, nullptr, &Parser::Binary, Precedence::Or)
DEFINE_TOKEN_RULE(And, nullptr, &Parser::Binary, Precedence::And)
DEFINE_TOKEN_RULE(BangEqual, nullptr, &Parser::Binary, Precedence::Equality)
DEFINE_TOKEN_RULE(EqualEqual, nullptr, &Parser::Binary, Precedence::Equality)
DEFINE_TOKEN_RULE(Greater, nullptr, &Parser::Binary, Precedence::Comparison)
DEFINE_TOKEN_RULE(GreaterEqual, nullptr, &Parser::Binary,
                  Precedence::Comparison)
DEFINE_TOKEN_RULE(Less, nullptr, &Parser::Binary, Precedence::Comparison)
DEFINE_TOKEN_RULE(LessEqual, nullptr, &Parser::Binary, Precedence::Comparison)
DEFINE_TOKEN_RULE(Minus, &Parser::Unary, &Parser::Binary, Precedence::Term)
DEFINE_TOKEN_RULE(Plus, nullptr, &Parser::Binary, Precedence::Term)
DEFINE_TOKEN_RULE(Star, nullptr, &Parser::Binary, Precedence::Factor)
DEFINE_TOKEN_RULE(Slash, nullptr, &Parser::Binary, Precedence::Factor)
DEFINE_TOKEN_RULE(Bang, &Parser::Unary, nullptr, Precedence::Unary)

ParseRule parse_rules[] = {
    TOKEN_RULE(0),  TOKEN_RULE(1),  TOKEN_RULE(2),  TOKEN_RULE(3),
    TOKEN_RULE(4),  TOKEN_RULE(5),  TOKEN_RULE(6),  TOKEN_RULE(7),
    TOKEN_RULE(8),  TOKEN_RULE(9),  TOKEN_RULE(10), TOKEN_RULE(11),
    TOKEN_RULE(12), TOKEN_RULE(13), TOKEN_RULE(14), TOKEN_RULE(15),
    TOKEN_RULE(16), TOKEN_RULE(17), TOKEN_RULE(18), TOKEN_RULE(19),
    TOKEN_RULE(20), TOKEN_RULE(21), TOKEN_RULE(22), TOKEN_RULE(23),
    TOKEN_RULE(24), TOKEN_RULE(25), TOKEN_RULE(26), TOKEN_RULE(27),
    TOKEN_RULE(28), TOKEN_RULE(29), TOKEN_RULE(30), TOKEN_RULE(31),
    TOKEN_RULE(32), TOKEN_RULE(33), TOKEN_RULE(34), TOKEN_RULE(35),
    TOKEN_RULE(36), TOKEN_RULE(37), TOKEN_RULE(38), TOKEN_RULE(39),
    TOKEN_RULE(40), TOKEN_RULE(41), TOKEN_RULE(42),
};
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

void Parser::_consume(TokenType type, const char *message) {
  if (_curr_token_t == type) {
    _advance();
    return;
  }

  _error_at_current(message);
}

void Parser::_type_error(ValueType expected, ValueType type,
                         const char *message) {
  if (expected != type) {
    _error(message);
  }
}

const ParseRule &Parser::_get_parse_rule(TokenType token_t) const {
  static_assert(sizeof(parse_rules) / sizeof(ParseRule) ==
                static_cast<int>(TokenType::Count));

  return parse_rules[static_cast<int>(token_t)];
}

void Parser::_parse_precedence(Precedence prec) {
  _advance();
  ParseFn prefix_rule = _get_parse_rule(_prev_token_t).prefix;
  if (prefix_rule == nullptr) {
    _error("Expect expression.");
    return;
  }

  (this->*prefix_rule)();

  while (prec <= _get_parse_rule(_curr_token_t).prec) {
    _advance();
    ParseFn infix_rule = _get_parse_rule(_prev_token_t).infix;
    (this->*infix_rule)();
  }
}

void Parser::Binary() {
  ValueType left_last_value_t = _last_value_t;

  TokenType operator_t = _prev_token_t;
  const ParseRule &rule = _get_parse_rule(operator_t);
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

void Parser::Expression() { _parse_precedence(Precedence::Assignment); }

void Parser::Grouping() {
  Expression();
  _consume(TokenType::CloseParen, "Expect ')' after expression.");
}

void Parser::Literal() {
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

void Parser::Number() {
  Value value = strtod(_prev_token.start, nullptr);
  _emit_constant(value);

  _last_value_t = ValueType::Number;
}

void Parser::String() {
  _emit_constant(new Str(_prev_token.start + 1, _prev_token.length - 2));

  _last_value_t = ValueType::Str;
}

void Parser::Unary() {
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
