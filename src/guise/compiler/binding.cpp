#include "binding.h"

using namespace GuiSE;

Scope::Scope(int offset) : _stack_frame_offset(offset) {}

bool Scope::AddVar(const std::string &id, ValueType type) { return false; }

const VarBinding *Scope::FindVar(const std::string &id) { return nullptr; }

Binding::Binding(const Id &id) : _id(id) {}

VarBinding::VarBinding(const Id &id, ValueType type, int offset)
    : Binding(id), _type(type), _offset(offset) {}

FnBinding::FnBinding(const Id& id, ValueType return_type, int byte_offset) : Binding(id), _return_type(return_type), _byte_offset(byte_offset)
{
}
