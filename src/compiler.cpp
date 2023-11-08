#include "compiler.h"

#include "byte_code.h"
#include "debug.h"
#include "scanner.h"
#include "types.h"

using namespace GuiSE;

bool Compiler::Compile(const char *source, ByteCode &byte_code) {
  _scanner.set_source(source);
  _byte_code = &byte_code;
  _advance();
  _expression();
  _consume(TokenType::Eof, "Expect end of expression.");
  return !_parser.had_error;
}

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

void Compiler::_expression() { _number(); }

void Compiler::_number() {
  GUISE_ASSERT(_byte_code != nullptr)

  Value value = {strtod(_parser.previous.start, nullptr)};
  int index = _byte_code->AddConstant(value);
  _emit_byte(OpCode::Constant);
  _emit_byte(index);
}

void Compiler::_emit_byte(uint8_t byte) {
  GUISE_ASSERT(_byte_code != nullptr)

  _byte_code->Write(byte);
}

void Compiler::_emit_byte(OpCode op_code) {
  _emit_byte(static_cast<uint8_t>(op_code));
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

using ParseFn = void (*)();

struct ParseRule {
  ParseFn prefix;
  ParseFn infix;
  Precedence precedence;
};
