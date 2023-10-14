#pragma once

#include "value.h"

#include <cstdint>
#include <vector>

enum class OpCode : uint8_t {
	CONSTANT,
	RETURN
};

class Chunk {
public:
	void write(uint8_t byte, int line);
	void write(OpCode opcode, int line);

	int addConstant(Value value);

	void disassemble(const char* name) const;
private:
	int disassembleInstruction(int offset) const;
	int constantInstruction(const char* name, int offset) const;

	std::vector<uint8_t> mCode;
	std::vector<int> mLines;
	ValueArray mConstants;
};