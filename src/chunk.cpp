#include "chunk.h"

namespace {
	int simpleInstruction(const char* name, int offset) {
		printf("%s\n", name);
		return offset + 1;
	}
}

void Chunk::write(uint8_t byte, int line) {
	mCode.push_back(byte);
	mLines.push_back(line);
}

void Chunk::write(OpCode opcode, int line) {
	write(static_cast<uint8_t>(opcode), line);
}

int Chunk::addConstant(Value value) {
	mConstants.push_back(value);
	return mConstants.size() - 1;
}

void Chunk::disassemble(const char* name) const {
	printf("== %s ==\n", name);

	for (int offset = 0; offset < mCode.size();) {
		offset = disassembleInstruction(offset);
	}
}

int Chunk::disassembleInstruction(int offset) const {
	printf("%04d ", offset);
	if (offset > 0 && mLines[offset] == mLines[offset - 1]) {
		printf("   | ");
	} else {
		printf("%4d ", mLines[offset]);
	}

	uint8_t instruction = mCode[offset];
	switch (static_cast<OpCode>(instruction)) {
	case OpCode::CONSTANT:
		return constantInstruction("CONSTANT", offset);
	case OpCode::RETURN:
		return simpleInstruction("RETURN", offset);
	default:
		printf("Unknown opcode %d\n", instruction);
		return offset + 1;
	}
}

int Chunk::constantInstruction(const char* name, int offset) const {
	uint8_t constant = mCode[offset + 1];
	printf("%-16s %4d '", name, constant);
	printValue(mConstants[constant]);
	printf("'\n");
	return offset + 2;
}
