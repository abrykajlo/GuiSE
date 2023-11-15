#include <guise/vm/vm.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

using namespace GuiSE;

namespace {
void repl(VM &vm) {
  std::string line;
  for (;;) {
    std::cout << "> ";
    std::getline(std::cin, line);

    if (line == "") {
      break;
    }

    vm.Interpret(line.c_str());
    std::cout << std::endl;
  }
}

void run_file(VM &vm, const char *file_name) {
  std::ifstream file(file_name);
  std::stringstream ss;
  ss << file.rdbuf();

  vm.Interpret(ss.str().c_str());
}
} // namespace

int main(int argc, const char *argv[]) {
  VM vm;
  if (argc == 1) {
    repl(vm);
  } else if (argc == 2) {
    run_file(vm, argv[1]);
  }

  return 0;
}