#pragma once

#include "types.h"

#include <map>
#include <set>
#include <string>
#include <vector>

namespace GuiSE {
using BindingId = std::string;

class VarBinding {
public:
  VarBinding() = default;
  VarBinding(ValueType type, int offset);
  ~VarBinding() = default;

  inline ValueType get_type() const { return _type; }
  inline int get_offset() const { return _offset; }

private:
  ValueType _type = ValueType::Invalid;
  int _offset = -1;
};

struct Param {
  const std::string id;
  ValueType type;
};

class FnBinding {
public:
  FnBinding(int constant, std::vector<Param> &params, ValueType return_type);

  inline ValueType get_return_type() const { return _return_type; }
  inline int get_byte_offset() const { return _constant; }

private:
  std::vector<Param> _params;
  int _constant;
  ValueType _return_type;
};

class Scope {
public:
  Scope();
  Scope(const Scope &scope);

  bool AddVar(const BindingId &id, ValueType type);
  const VarBinding *FindVar(const BindingId &id) const;

  using VarBindingMap = std::map<BindingId, VarBinding>;

  VarBindingMap::const_iterator begin() const;
  VarBindingMap::const_iterator end() const;

private:
  VarBindingMap _var_bindings;
  int _stack_frame_offset;
};

class ScopeStack {
public:
  bool AddFn(const BindingId &id, int byte_offset, std::vector<Param> &params,
             ValueType return_type);
  bool AddVar(const BindingId &id, ValueType type);

  const FnBinding *FindFn(const BindingId &id);
  const VarBinding *FindVar(const BindingId &id, bool &is_global) const;

  void PushFnScope();
  void PushBlockScope();

  void Pop();

private:
  inline bool _has_binding(const BindingId &id) const {
    return _unique_ids.find(id) != _unique_ids.end();
  }

  inline void _add_binding(const BindingId &id) { _unique_ids.insert(id); }

  std::vector<Scope> _stack;
  std::set<BindingId> _unique_ids;

  // global bindings
  int _global_offset = 0;
  std::map<BindingId, FnBinding> _fn_bindings;
  std::map<BindingId, VarBinding> _var_bindings;
};
} // namespace GuiSE