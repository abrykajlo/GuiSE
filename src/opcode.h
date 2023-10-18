#pragma once

#include "types.h"

#include <cstdint>
#include <functional>

#define _OPERANDS(OPCODE, ...) \
	template <> \
	struct Operands<OpCode::OPCODE, __VA_ARGS__> : std::true_type {};

#define _MATCH_OPS(OPTYPE, ...) \
	typename = std::enable_if_t<Operands<OPTYPE, __VA_ARGS__>::value>

namespace GuiSE {
	enum class OpCode : u8 {
		NOOP,
		LINE,
		CONSTANT,
		RETURN,
	};

	namespace internal {
		template <OpCode I, typename ...T>
		struct Operands : std::false_type {};

		_OPERANDS(NOOP)
		_OPERANDS(RETURN)
		_OPERANDS(LINE, u8)
		_OPERANDS(CONSTANT, u8)
	}

	using namespace internal;

	template <typename Reader>
	class OpCodeReader {
	public:
		OpCodeReader(Reader& reader) : _reader(reader) {}

		OpCode read() {

		}

	private:
		u64 offset = 0;
		Reader& _reader;
	};

	template <typename Writer>
	class OpCodeWriter {
	public:
		OpCodeWriter(Writer& writer) : _writer(writer) {}

		template <OpCode I, _MATCH_OPS(I)>
		void write() {
			_write<I>();
		}

		template <OpCode I, _MATCH_OPS(I, u8)>
		void write(u8 byte) {
			_write<I>();
			_write(byte);
		}

	private:
		template <OpCode I>
		inline void _write() {
			_writer.write(static_cast<u8>(I));
		}

		inline void _write(u8 byte) {
			_writer.write(byte);
		}

		Writer& _writer;
	};
}

#undef _MATCH_OPS
#undef _OPERANDS
