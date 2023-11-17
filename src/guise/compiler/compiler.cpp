#include "compiler.h"

#include "parser.h"
#include "scanner.h"

#include <guise/vm/byte_code.h>

bool GuiSE::compile(const char *source, ByteCode &byte_code) {
  Scanner scanner(source);
  Parser parser(scanner, byte_code);
  
  while (!parser.IsDone()) {
	  parser.Declaration();
  }

  return !parser.get_had_error();
}
