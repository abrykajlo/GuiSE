#include "compiler.h"

#include "byte_code.h"
#include "scanner.h"

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

using namespace GuiSE;

class Parser {
	Parser();

	void Advance();

private:
	Token _current;
	Token _previous;
	Scanner _scanner;
};

using ParseFn = void(*)();

struct ParseRule {
	ParseFn prefix;
	ParseFn infix;
	Precedence precedence;
};

bool GuiSE::compile(const char *source, ByteCode &byte_code) { 
	Scanner scanner;
	scanner.set_source(source);
	return false; 
}

