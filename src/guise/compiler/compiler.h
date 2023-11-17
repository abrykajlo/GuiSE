#pragma once

namespace GuiSE {
class ByteCode;

bool compile(const char *source, ByteCode &byte_code);
} // namespace GuiSE