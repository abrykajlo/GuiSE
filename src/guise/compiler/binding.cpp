#include "binding.h"

using namespace GuiSE;

Scope::Scope() : _stack_frame_offset(0) {}

Scope::Scope(const Scope &scope)
    : _stack_frame_offset(scope._stack_frame_offset) {}

bool Scope::AddVar(const BindingId &id, ValueType type) {
  auto it = _var_bindings.find(id);
  if (it != _var_bindings.end()) {
    return false;
  }
  _var_bindings[id] = VarBinding(type, _stack_frame_offset++);
  return true;
}

const VarBinding *Scope::FindVar(const BindingId &id) const {
  auto it = _var_bindings.find(id);
  if (it != _var_bindings.end()) {
    return &it->second;
  }
  return nullptr;
}

Scope::VarBindingMap::const_iterator Scope::begin() const {
  return _var_bindings.begin();
}

Scope::VarBindingMap::const_iterator Scope::end() const {
  return _var_bindings.end();
}

VarBinding::VarBinding(ValueType type, int offset)
    : _type(type), _offset(offset) {}

FnBinding::FnBinding(int constant, std::vector<Param> &params,
                     ValueType return_type)
    : _constant(constant), _params(std::move(params)),
      _return_type(return_type) {}

bool ScopeStack::AddFn(const BindingId &id, int constant,
                       std::vector<Param> &params, ValueType return_type) {
  if (!_stack.empty())
    return false;
  if (_has_binding(id)) {
    return false;
  }
  _add_binding(id);
  _fn_bindings.insert({id, FnBinding(constant, params, return_type)});
  return true;
}

bool ScopeStack::AddVar(const BindingId &id, ValueType type) {
  if (_has_binding(id)) {
    return false;
  }
  _add_binding(id);
  if (_stack.empty()) {
    _var_bindings[id] = VarBinding(type, _global_offset++);
  } else {
    _stack.back().AddVar(id, type);
  }
  return true;
}

const FnBinding *ScopeStack::FindFn(const BindingId &id) {
  if (!_has_binding(id)) {
    return nullptr;
  }
  auto it = _fn_bindings.find(id);
  if (it != _fn_bindings.end()) {
    return &it->second;
  }
  return nullptr;
}

const VarBinding *ScopeStack::FindVar(const BindingId &id,
                                      bool &is_global) const {
  if (!_has_binding(id)) {
    return nullptr;
  }
  for (auto it = _stack.crbegin(); it != _stack.crend(); it++) {
    const VarBinding *var_binding = it->FindVar(id);
    if (var_binding != nullptr) {
      is_global = false;
      return var_binding;
    }
  }
  auto it = _var_bindings.find(id);
  if (it != _var_bindings.end()) {
    is_global = true;
    return &it->second;
  }
  return nullptr;
}

void ScopeStack::PushFnScope() { _stack.emplace_back(); }

void ScopeStack::PushBlockScope() { _stack.emplace_back(_stack.back()); }

void ScopeStack::Pop() {
  for (const auto &var : _stack.back()) {
    _unique_ids.erase(var.first);
  }
  _stack.pop_back();
}
