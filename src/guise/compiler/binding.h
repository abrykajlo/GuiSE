#pragma once

#include "types.h"

#include <map>
#include <memory>
#include <stack>
#include <string>
#include <utility>

namespace GuiSE {
class Binding {
public:
  using Id = std::string;

  Binding(const Id &id);
  virtual ~Binding() = default;

  inline const Id &get_id() const { return _id; }

private:
  const Id _id;
};

class VarBinding : Binding {
public:
  VarBinding(const Id &id, ValueType type, int offset);
  virtual ~VarBinding() = default;

  inline ValueType get_type() const { return _type; }
  inline int get_offset() const { return _offset; }

private:
  ValueType _type;
  int _offset;
};

class FnBinding : Binding {
public:
	FnBinding(const Id& id, ValueType return_type, int byte_offset);

	inline ValueType get_return_type() const { return _return_type; }
	inline int get_byte_offset() const { return _byte_offset; }

private:
  ValueType _return_type;
  int _byte_offset;
};

class Scope {
public:
  Scope(int offset = 0);

  bool AddVar(const std::string &id, ValueType type);
  const VarBinding *FindVar(const std::string &id);

private:
	std::map<Binding::Id, std::unique_ptr<VarBinding>> _var_bindings;
  int _stack_frame_offset;
};

class ScopeStack {
public:
  bool AddBinding(const std::string &id, std::unique_ptr<Binding>&& binding);
  const Binding *FindBinding(const std::string &id) const;

private:
  std::stack<Scope> _scopes;
  std::map<Binding::Id, std::unique_ptr<Binding>> _global_bindings;
};
} // namespace GuiSE