#include "chunk.h"
#include "opcode.h"
#include "vm.h"

using namespace GuiSE;

int main(int argc, const char* argv[]) {
	VM vm;
	Chunk chunk;
	OpCodeWriter opcodeWriter(chunk);
	opcodeWriter.write<OpCode::LINE>(123);
	
	int constant = chunk.addConstant(1.2);
	opcodeWriter.write<OpCode::CONSTANT>(constant);
	opcodeWriter.write<OpCode::RETURN>();
	chunk.disassemble("test chunk");
	//vm.interpret(&chunk);
	return 0;
}