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
  const BindingId id;
  ValueType type;
};

class FnBinding {
public:
  FnBinding(int constant, const std::vector<Param> &params,
            ValueType return_type);

  inline const std::vector<Param> &get_params() const { return _params; }
  inline int get_constant() const { return _constant; }
  inline ValueType get_return_type() const { return _return_type; }

private:
  std::vector<Param> _params;
  int _constant;
  ValueType _return_type;
};

class Scope {
public:
  Scope();
  Scope(const Scope &scope);

  void AddVar(const BindingId &id, ValueType type);
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
  bool AddFn(const BindingId &id, int byte_offset,
             const std::vector<Param> &params, ValueType return_type);
  void AddVar(const BindingId &id, ValueType type);

  const FnBinding *FindFn(const BindingId &id);
  const VarBinding *FindVar(const BindingId &id, bool &is_global) const;

  void PushFnScope();
  void PushBlockScope();

  void Pop();

private:
  std::vector<Scope> _stack;

  // global bindings
  std::map<BindingId, FnBinding> _fn_bindings;
  std::map<BindingId, VarBinding> _var_bindings;
  int _global_offset = 0;
};
} // namespace GuiSE