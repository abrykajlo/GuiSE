#include "compiler.h"

#include "byte_code.h"
#include "debug.h"
#include "object.h"
#include "scanner.h"
#include "types.h"

#define DEFINE_TOKEN_RULE(TOKEN, ...)                                          \
  template <> ParseRule Compiler::_parse_rule<TokenType::TOKEN> = {__VA_ARGS__};

#define TOKEN_RULE(INDEX) _parse_rule<static_cast<TokenType>(INDEX)>

namespace {
const char *expect_number_operand = "Expect number type operand.";
const char *expect_bool_operand = "Expect bool type operand.";

const char *expect_number_left_operand = "Expect number type left operand.";
const char *expect_number_right_operand = "Expect number type right operand.";

const char *expect_bool_left_operand = "Expect bool type left operand.";
const char *expect_bool_right_operand = "Expect bool type right operand.";
} // namespace

using namespace GuiSE;

bool Compiler::Compile(const char *source, ByteCode &byte_code) {
  _scanner.set_source(source);
  _byte_code = &byte_code;
  _advance();
  _expression();
  _consume(TokenType::Eof, "Expect end of expression.");
  _emit_byte(OpCode::Log);
  _emit_byte(_parser.value_type);
  return !_parser.had_error;
}

DEFINE_TOKEN_RULE(True, &Compiler::_literal)
DEFINE_TOKEN_RULE(False, &Compiler::_literal)
DEFINE_TOKEN_RULE(Number, &Compiler::_number)
DEFINE_TOKEN_RULE(OpenParen, &Compiler::_grouping)
DEFINE_TOKEN_RULE(String, &Compiler::_string)
DEFINE_TOKEN_RULE(Or, nullptr, &Compiler::_binary, Precedence::Or)
DEFINE_TOKEN_RULE(And, nullptr, &Compiler::_binary, Precedence::And)
DEFINE_TOKEN_RULE(BangEqual, nullptr, &Compiler::_binary, Precedence::Equality)
DEFINE_TOKEN_RULE(EqualEqual, nullptr, &Compiler::_binary, Precedence::Equality)
DEFINE_TOKEN_RULE(Greater, nullptr, &Compiler::_binary, Precedence::Comparison)
DEFINE_TOKEN_RULE(GreaterEqual, nullptr, &Compiler::_binary,
                  Precedence::Comparison)
DEFINE_TOKEN_RULE(Less, nullptr, &Compiler::_binary, Precedence::Comparison)
DEFINE_TOKEN_RULE(LessEqual, nullptr, &Compiler::_binary,
                  Precedence::Comparison)
DEFINE_TOKEN_RULE(Minus, &Compiler::_unary, &Compiler::_binary,
                  Precedence::Term)
DEFINE_TOKEN_RULE(Plus, nullptr, &Compiler::_binary, Precedence::Term)
DEFINE_TOKEN_RULE(Star, nullptr, &Compiler::_binary, Precedence::Factor)
DEFINE_TOKEN_RULE(Slash, nullptr, &Compiler::_binary, Precedence::Factor)
DEFINE_TOKEN_RULE(Bang, &Compiler::_unary, nullptr, Precedence::Unary)

ParseRule Compiler::_parse_rules[] = {
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

void Compiler::_advance() {
  _parser.prev = _parser.curr;

  for (;;) {
    _parser.curr = _scanner.ScanToken();
    if (_parser.curr.type != TokenType::Error)
      break;

    _error_at_current(_parser.curr.start);
  }
}

void Compiler::_consume(TokenType type, const char *message) {
  if (_parser.curr.type == type) {
    _advance();
    return;
  }

  _error_at_current(message);
}

void Compiler::_type_error(ValueType expected, ValueType type,
                           const char *message) {
  if (expected != type) {
    _error(message);
  }
}

const ParseRule &Compiler::_get_parse_rule(TokenType token_type) const {
  static_assert(sizeof(_parse_rules) / sizeof(ParseRule) ==
                static_cast<int>(TokenType::Count));

  return _parse_rules[static_cast<int>(token_type)];
}

void Compiler::_parse_precedence(Precedence prec) {
  _advance();
  ParseFn prefix_rule = _get_parse_rule(_parser.prev.type).prefix;
  if (prefix_rule == nullptr) {
    _error("Expect expression.");
    return;
  }

  (this->*prefix_rule)();

  while (prec <= _get_parse_rule(_parser.curr.type).prec) {
    _advance();
    ParseFn infix_rule = _get_parse_rule(_parser.prev.type).infix;
    (this->*infix_rule)();
  }
}

void Compiler::_binary() {
  ValueType left_value_type = _parser.value_type;

  TokenType operator_type = _parser.prev.type;
  const ParseRule &rule = _get_parse_rule(operator_type);
  _parse_precedence(static_cast<Precedence>(static_cast<int>(rule.prec) + 1));
  ValueType right_value_type = _parser.value_type;

  ValueType expected_value_type = ValueType::Invalid;
  ValueType return_type = ValueType::Invalid;
  switch (operator_type) {
  case TokenType::Plus:
    expected_value_type = ValueType::Number;
    _emit_byte(OpCode::Add);
    return_type = ValueType::Number;
    break;
  case TokenType::Minus:
    expected_value_type = ValueType::Number;
    _emit_byte(OpCode::Negate);
    _emit_byte(OpCode::Add);
    return_type = ValueType::Number;
    break;
  case TokenType::Star:
    expected_value_type = ValueType::Number;
    _emit_byte(OpCode::Multiply);
    return_type = ValueType::Number;
    break;
  case TokenType::Slash:
    expected_value_type = ValueType::Number;
    _emit_byte(OpCode::Divide);
    return_type = ValueType::Number;
    break;
  case TokenType::BangEqual:
    expected_value_type = ValueType::Number;
    _emit_byte(OpCode::Equal);
    _emit_byte(OpCode::Not);
    return_type = ValueType::Bool;
    break;
  case TokenType::EqualEqual:
    expected_value_type = ValueType::Number;
    _emit_byte(OpCode::Equal);
    return_type = ValueType::Bool;
    break;
  case TokenType::Greater:
    expected_value_type = ValueType::Number;
    _emit_byte(OpCode::Greater);
    return_type = ValueType::Bool;
    break;
  case TokenType::GreaterEqual:
    expected_value_type = ValueType::Number;
    _emit_byte(OpCode::Less);
    _emit_byte(OpCode::Not);
    return_type = ValueType::Bool;
    break;
  case TokenType::Less:
    expected_value_type = ValueType::Number;
    _emit_byte(OpCode::Less);
    return_type = ValueType::Bool;
    break;
  case TokenType::LessEqual:
    expected_value_type = ValueType::Number;
    _emit_byte(OpCode::Greater);
    _emit_byte(OpCode::Not);
    return_type = ValueType::Bool;
    break;
  case TokenType::Or:
    expected_value_type = ValueType::Bool;
    _emit_byte(OpCode::Or);
    return_type = ValueType::Bool;
    break;
  case TokenType::And:
    expected_value_type = ValueType::Bool;
    _emit_byte(OpCode::And);
    return_type = ValueType::Bool;
    break;
  }

  const char *expect_left_message = nullptr;
  const char *expect_right_message = nullptr;
  if (expected_value_type == ValueType::Bool) {
    expect_left_message = expect_bool_left_operand;
    expect_right_message = expect_bool_right_operand;
  } else if (expected_value_type == ValueType::Number) {
    expect_left_message = expect_number_left_operand;
    expect_right_message = expect_number_right_operand;
  }
  _type_error(expected_value_type, left_value_type, expect_left_message);
  _type_error(expected_value_type, right_value_type, expect_right_message);

  _parser.value_type = return_type;
}

void Compiler::_expression() { _parse_precedence(Precedence::Assignment); }

void Compiler::_grouping() {
  _expression();
  _consume(TokenType::CloseParen, "Expect ')' after expression.");
}

void Compiler::_literal() {
  switch (_parser.prev.type) {
  case TokenType::True:
    _emit_byte(OpCode::True);
    _parser.value_type = ValueType::Bool;
    break;
  case TokenType::False:
    _emit_byte(OpCode::False);
    _parser.value_type = ValueType::Bool;
    break;
  }
}

void Compiler::_number() {
  Value value = strtod(_parser.prev.start, nullptr);
  _emit_constant(value);

  _parser.value_type = ValueType::Number;
}

void Compiler::_string() {
  _emit_constant(new String(_parser.prev.start + 1, _parser.prev.length - 2));

  _parser.value_type = ValueType::Str;
}

void Compiler::_unary() {
  TokenType operator_type = _parser.prev.type;

  _parse_precedence(Precedence::Unary);
  ValueType value_type = _parser.value_type;

  switch (operator_type) {
  case TokenType::Minus:
    _type_error(ValueType::Number, value_type, "Expect number type operand.");
    _emit_byte(OpCode::Negate);
    break;
  case TokenType::Bang:
    _type_error(ValueType::Bool, value_type, "Expect bool type operand.");
    _emit_byte(OpCode::Not);
    break;
  }
}

void Compiler::_emit_byte(uint8_t byte) {
  GUISE_ASSERT(_byte_code != nullptr)

  _byte_code->Write(byte);
}

void Compiler::_emit_constant(Value value) {
  _emit_byte(OpCode::Constant);
  _emit_byte(_make_constant(value));
}

uint8_t Compiler::_make_constant(Value value) {
  GUISE_ASSERT(_byte_code != nullptr)

  int constant = _byte_code->AddConstant(value);
  if (constant > std::numeric_limits<uint8_t>::max()) {
    _error("Too many constants in one chunk.");
    return 0;
  }

  return static_cast<uint8_t>(constant);
}

void Compiler::_error_at(const Token &token, const char *message) {
  fprintf(stderr, "[line %d] Error", token.line);

  if (token.type == TokenType::Eof) {
    fprintf(stderr, " at end");
  } else if (token.type == TokenType::Error) {
    // Nothing.
  } else {
    fprintf(stderr, " at '%.*s'", token.length, token.start);
  }

  fprintf(stderr, ": %s\n", message);
  _parser.had_error = true;
}

void Compiler::_error_at_current(const char *message) {
  _error_at(_parser.curr, message);
}

void Compiler::_error(const char *message) { _error_at(_parser.prev, message); }
