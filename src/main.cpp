#include "chunk.h"

int main(int argc, const char* argv[]) {
	Chunk chunk;
	int constant = chunk.addConstant(1.2);
	chunk.write(OpCode::CONSTANT, 123);
	chunk.write(constant, 123);
	chunk.write(OpCode::RETURN, 123);
	chunk.disassemble("test chunk");
	return 0;
}