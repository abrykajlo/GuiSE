#include "vm.h"

#include <iostream>
#include <string>

using namespace GuiSE;

static void repl() {
  VM vm;
  std::string line;
  for (;;) {
    std::cout << "> ";
    std::getline(std::cin, line);

    if (line == "") {
      break;
    }

    vm.Interpret(line.c_str());
  }
}

int main(int argc, const char *argv[]) {
  if (argc == 1) {
    repl();
  }

  return 0;
}