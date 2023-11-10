#include "compiler.h"

#include "byte_code.h"
#include "debug.h"
#include "scanner.h"
#include "types.h"

#define TOKEN_RULE(INDEX) _parse_rule<static_cast<TokenType>(INDEX)>

#define DEFINE_TOKEN_RULE(TOKEN, ...)                                          \
  template <> ParseRule Compiler::_parse_rule<TokenType::TOKEN> = {__VA_ARGS__};

using namespace GuiSE;

bool Compiler::Compile(const char *source, ByteCode &byte_code) {
  _scanner.set_source(source);
  _byte_code = &byte_code;
  _advance();
  _expression();
  _consume(TokenType::Eof, "Expect end of expression.");
  _emit_byte(OpCode::Log);
  _emit_byte(ValueType::Number);
  return !_parser.had_error;
}

DEFINE_TOKEN_RULE(Number, &Compiler::_number)
DEFINE_TOKEN_RULE(OpenParen, &Compiler::_grouping)
DEFINE_TOKEN_RULE(Minus, &Compiler::_unary, &Compiler::_binary,
                  Precedence::Term)
DEFINE_TOKEN_RULE(Plus, nullptr, &Compiler::_binary, Precedence::Term)
DEFINE_TOKEN_RULE(Star, nullptr, &Compiler::_binary, Precedence::Factor)
DEFINE_TOKEN_RULE(Slash, nullptr, &Compiler::_binary, Precedence::Factor)

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
  _parser.previous = _parser.current;

  for (;;) {
    _parser.current = _scanner.ScanToken();
    if (_parser.current.type != TokenType::Error)
      break;

    _error_at_current(_parser.current.start);
  }
}

void Compiler::_consume(TokenType type, const char *message) {
  if (_parser.current.type == type) {
    _advance();
    return;
  }

  _error_at_current(message);
}

const ParseRule &Compiler::_get_parse_rule(TokenType token_type) const {
  static_assert(sizeof(_parse_rules) / sizeof(ParseRule) ==
                static_cast<int>(TokenType::Count));

  return _parse_rules[static_cast<int>(token_type)];
}

void Compiler::_parse_precedence(Precedence prec) {
  _advance();
  ParseFn prefix_rule = _get_parse_rule(_parser.previous.type).prefix;
  if (prefix_rule == nullptr) {
    _error("Expect expression.");
    return;
  }

  (this->*prefix_rule)();

  while (prec <= _get_parse_rule(_parser.current.type).prec) {
    _advance();
    ParseFn infix_rule = _get_parse_rule(_parser.previous.type).infix;
    (this->*infix_rule)();
  }
}

void Compiler::_binary() {
  TokenType operator_type = _parser.previous.type;
  const ParseRule &rule = _get_parse_rule(operator_type);
  _parse_precedence(static_cast<Precedence>(static_cast<int>(rule.prec) + 1));

  switch (operator_type) {
  case TokenType::Plus:
    _emit_byte(OpCode::Add);
    break;
  case TokenType::Minus:
    _emit_byte(OpCode::Negate);
    _emit_byte(OpCode::Add);
    break;
  case TokenType::Star:
    _emit_byte(OpCode::Multiply);
    break;
  case TokenType::Slash:
    _emit_byte(OpCode::Divide);
    break;
  default:
    return;
  }
}

void Compiler::_expression() { _parse_precedence(Precedence::Assignment); }

void Compiler::_grouping() {
  _expression();
  _consume(TokenType::CloseParen, "Expect ')' after expression.");
}

void Compiler::_number() {
  GUISE_ASSERT(_byte_code != nullptr)

  Value value = strtod(_parser.previous.start, nullptr);
  int index = _byte_code->AddConstant(value);
  _emit_byte(OpCode::Constant);
  _emit_byte(index);
}

void Compiler::_unary() {
  TokenType operator_type = _parser.previous.type;

  _parse_precedence(Precedence::Unary);

  switch (operator_type) {
  case TokenType::Minus:
    _emit_byte(OpCode::Negate);
    break;
  default:
    return;
  }
}

void Compiler::_emit_byte(uint8_t byte) {
  GUISE_ASSERT(_byte_code != nullptr)

  _byte_code->Write(byte);
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
  _error_at(_parser.current, message);
}

void Compiler::_error(const char *message) {
  _error_at(_parser.previous, message);
}
