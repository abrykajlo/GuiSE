#include "compiler.h"

#include "disassembler.h"
#include "parser.h"
#include "scanner.h"

#include <guise/debug.h>
#include <guise/vm/byte_code.h>

bool GuiSE::compile(const char *source, ByteCode &byte_code) {
  Scanner scanner(source);
  Parser parser(scanner, byte_code);
  const bool success = parser.Parse();
#ifdef GUISE_DEBUG
  if (success) {
    disassemble(byte_code);
  }
#endif
  return success;
}
