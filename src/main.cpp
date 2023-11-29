#include <guise/compiler/compiler.h>
#include <guise/vm/byte_code.h>
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

    ByteCode byte_code;
    compile(line.c_str(), byte_code);
    vm.set_byte_code(byte_code);
    vm.Run();
    std::cout << std::endl;
  }
}

void run_file(VM &vm, const char *file_name) {
  std::ifstream file(file_name);
  std::stringstream ss;
  ss << file.rdbuf();

  ByteCode byte_code;
  if (!compile(ss.str().c_str(), byte_code)) {
    return;
  }
  vm.set_byte_code(byte_code);
  vm.Call("main");
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