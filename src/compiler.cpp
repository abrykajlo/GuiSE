#include "compiler.h"

#include "byte_code.h"
#include "scanner.h"

using namespace GuiSE;

namespace {
class Parser {
public:
  Parser(const char *source, ByteCode &byte_code);

  bool get_had_error();

private:
  void _advance();

  void _error_at(const Token &token, const char *message);
  void _error_at_current(const char *message);
  void _error(const char *message);

  Token _current;
  Token _previous;
  bool _had_error = false;
  Scanner _scanner;
  ByteCode &_byte_code;
};

Parser::Parser(const char *source, ByteCode &byte_code)
    : _byte_code(byte_code) {
  _scanner.set_source(source);
  _advance();
}

bool Parser::get_had_error() { return _had_error; }

void Parser::_advance() {
  _previous = _current;

  for (;;) {
    _current = _scanner.ScanToken();
    if (_current.type != TokenType::Error)
      break;

    _error_at_current(_current.start);
  }
}

void Parser::_error_at(const Token &token, const char *message) {
  fprintf(stderr, "[line %d] Error", token.line);

  if (token.type == TokenType::Eof) {
    fprintf(stderr, " at end");
  } else if (token.type == TokenType::Error) {
    // Nothing.
  } else {
    fprintf(stderr, " at '%.*s'", token.length, token.start);
  }

  fprintf(stderr, ": %s\n", message);
  _had_error = true;
}

void Parser::_error_at_current(const char *message) {
  _error_at(_current, message);
}

void Parser::_error(const char *message) { _error_at(_previous, message); }
} // namespace

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

bool GuiSE::compile(const char *source, ByteCode &byte_code) {
  Parser parser(source, byte_code);
  return false;
}
