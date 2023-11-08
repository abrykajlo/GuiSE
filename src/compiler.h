#pragma once

#include "opcode.h"
#include "scanner.h"

namespace GuiSE {
class ByteCode;

struct Parser {
  Token current;
  Token previous;
  bool had_error = false;
  bool panic_mode = false;
};

class Compiler {
public:
  bool Compile(const char *source, ByteCode &byte_code);

private:
  void _advance();
  void _consume(TokenType type, const char *message);

  void _expression();
  void _number();

  void _emit_byte(uint8_t byte);
  void _emit_byte(OpCode op_code);

  void _error_at(const Token &token, const char *message);
  void _error_at_current(const char *message);
  void _error(const char *message);

  Parser _parser;
  Scanner _scanner;
  ByteCode *_byte_code = nullptr;
};
} // namespace GuiSE