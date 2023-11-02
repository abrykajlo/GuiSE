#include "scanner.h"

#include <iostream>
#include <string>

using namespace GuiSE;

static void repl() {
  Scanner scanner;
  std::string line;
  for (;;) {
    std::cout << "> ";
    std::getline(std::cin, line);

    if (line == "") {
      break;
    }

    scanner.set_source(line);
    Token token = scanner.ScanToken();
    std::cout << std::string(token.start, token.length) << std::endl;
    while (token.type != TokenType::Eof && token.type != TokenType::Error) {
      token = scanner.ScanToken();
      std::cout << std::string(token.start, token.length) << std::endl;
    }
  }
}

int main(int argc, const char *argv[]) {
  if (argc == 1) {
    repl();
  }

  return 0;
}