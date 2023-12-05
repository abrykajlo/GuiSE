#include "parser.h"

#include <guise/debug.h>
#include <guise/vm/byte_code.h>
#include <guise/vm/object.h>
#include <guise/vm/opcode.h>

using namespace GuiSE;

namespace {
const char *expect_operand_type = "Expect %s type for operand, got %s.";

const char *expect_left_operand_type =
    "Expect %s type for left operand, got %s.";
const char *expect_right_operand_type =
    "Expect %s type for right operand, got %s.";

const char *expect_var_declaration_type =
    "Expect %s type for variable declaration expression, got %s.";
const char *expect_var_assignment_type =
    "Expect %s type for variable assignment expression, got %s.";
const char *expect_return_expr_type =
    "Expect %s type for return statement expression, got %s.";
const char *expect_fn_arg_type =
    "Expect %s type for function argument type, got %s.";

const char *expect_statment_semi_colon = "Expect ';' after statement.";

const char *identifier_bound = "Identifier already bound to.";
const char *identifier_not_bound = "Identifier is not bound to.";
} // namespace

void Parser::_advance() {
  _prev_token_type = _curr_token_type;
  _prev_token = _curr_token;

  for (;;) {
    _curr_token_type = _scanner.ScanToken(_curr_token);
    if (_curr_token_type != TokenType::Error)
      break;

    _error_at_current(_curr_token.start);
  }
}

bool Parser::_check(TokenType token_type) {
  return _curr_token_type == token_type;
}

void Parser::_consume(TokenType token_type, const char *message) {
  if (_curr_token_type == token_type) {
    _advance();
    return;
  }

  _error_at_current(message);
}

bool Parser::_match(TokenType token_type) {
  if (_check(token_type)) {
    _advance();
    return true;
  }
  return false;
}

void Parser::_type_error(ValueType expected, ValueType type,
                         const char *message) {
  if (expected != type) {
    char buf[256];
    snprintf(buf, sizeof(buf), message, type_string(expected),
             type_string(type));
    _error(buf);
  }
}

Parser::ParsePrefixFn Parser::_get_prefix_rule(TokenType token_type) const {
  switch (token_type) {
  case TokenType::True:
  case TokenType::False:
    return &Parser::_literal;
  case TokenType::Bang:
  case TokenType::Minus:
    return &Parser::_unary;
  case TokenType::Number:
    return &Parser::_number;
  case TokenType::OpenParen:
    return &Parser::_grouping;
  case TokenType::String:
    return &Parser::_string;
  case TokenType::Identifier:
    return &Parser::_identifier;
  }

  return nullptr;
}

const Parser::InfixRule &Parser::_get_infix_rule(TokenType token_type) const {
  static InfixRule or_rule = {&Parser::_binary, Precedence::Or};
  static InfixRule and_rule{&Parser::_binary, Precedence::And};
  static InfixRule equality_rule = {&Parser::_binary, Precedence::Equality};
  static InfixRule comparison_rule = {&Parser::_binary, Precedence::Comparison};
  static InfixRule term_rule = {&Parser::_binary, Precedence::Term};
  static InfixRule factor_rule = {&Parser::_binary, Precedence::Factor};

  switch (token_type) {
  case TokenType::Or:
    return or_rule;
  case TokenType::And:
    return and_rule;
  case TokenType::BangEqual:
  case TokenType::EqualEqual:
    return equality_rule;
  case TokenType::Greater:
  case TokenType::GreaterEqual:
  case TokenType::Less:
  case TokenType::LessEqual:
    return comparison_rule;
  case TokenType::Minus:
  case TokenType::Plus:
    return term_rule;
  case TokenType::Star:
  case TokenType::Slash:
    return factor_rule;
  }

  static InfixRule empty;
  return empty;
}

ValueType Parser::_parse_precedence(Precedence prec) {
  _advance();
  ParsePrefixFn prefix_rule = _get_prefix_rule(_prev_token_type);
  if (prefix_rule == nullptr) {
    _error("Expect expression.");
    return ValueType::Invalid;
  }

  ValueType type = (this->*prefix_rule)();

  const InfixRule *infix_rule = &_get_infix_rule(_curr_token_type);
  while (prec <= infix_rule->prec) {
    _advance();
    const ParseInfixFn rule = infix_rule->rule;
    type = (this->*rule)(type);
    infix_rule = &_get_infix_rule(_curr_token_type);
  }

  return type;
}

ValueType Parser::_binary(ValueType left_type) {
  TokenType op_token_type = _prev_token_type;
  const InfixRule &rule = _get_infix_rule(op_token_type);
  ValueType right_type = _parse_precedence(
      static_cast<Precedence>(static_cast<int>(rule.prec) + 1));

  ValueType expected_type = ValueType::Invalid;
  ValueType return_type = ValueType::Invalid;
  switch (op_token_type) {
  case TokenType::Plus:
    expected_type = ValueType::Num;
    _emit_byte(OpCode::Add);
    return_type = ValueType::Num;
    break;
  case TokenType::Minus:
    expected_type = ValueType::Num;
    _emit_byte(OpCode::Negate);
    _emit_byte(OpCode::Add);
    return_type = ValueType::Num;
    break;
  case TokenType::Star:
    expected_type = ValueType::Num;
    _emit_byte(OpCode::Multiply);
    return_type = ValueType::Num;
    break;
  case TokenType::Slash:
    expected_type = ValueType::Num;
    _emit_byte(OpCode::Divide);
    return_type = ValueType::Num;
    break;
  case TokenType::BangEqual:
    expected_type = ValueType::Num;
    _emit_byte(OpCode::Equal);
    _emit_byte(OpCode::Not);
    return_type = ValueType::Bool;
    break;
  case TokenType::EqualEqual:
    expected_type = ValueType::Num;
    _emit_byte(OpCode::Equal);
    return_type = ValueType::Bool;
    break;
  case TokenType::Greater:
    expected_type = ValueType::Num;
    _emit_byte(OpCode::Greater);
    return_type = ValueType::Bool;
    break;
  case TokenType::GreaterEqual:
    expected_type = ValueType::Num;
    _emit_byte(OpCode::Less);
    _emit_byte(OpCode::Not);
    return_type = ValueType::Bool;
    break;
  case TokenType::Less:
    expected_type = ValueType::Num;
    _emit_byte(OpCode::Less);
    return_type = ValueType::Bool;
    break;
  case TokenType::LessEqual:
    expected_type = ValueType::Num;
    _emit_byte(OpCode::Greater);
    _emit_byte(OpCode::Not);
    return_type = ValueType::Bool;
    break;
  case TokenType::Or:
    expected_type = ValueType::Bool;
    _emit_byte(OpCode::Or);
    return_type = ValueType::Bool;
    break;
  case TokenType::And:
    expected_type = ValueType::Bool;
    _emit_byte(OpCode::And);
    return_type = ValueType::Bool;
    break;
  }

  _type_error(expected_type, left_type, expect_left_operand_type);
  _type_error(expected_type, right_type, expect_right_operand_type);

  return return_type;
}

Parser::Parser(Scanner &scanner, ByteCode &byte_code)
    : _scanner(scanner), _byte_code(&byte_code) {
  _advance();
}

bool Parser::Parse() {
  _emit_byte(OpCode::Global);
  while (!_match(TokenType::Eof)) {
    _global_declaration();
  }
  _emit_byte(OpCode::NoOp);

  return !_had_error;
}

bool Parser::_match_id(std::string &identifier) {
  if (_match(TokenType::Identifier)) {
    identifier = std::string(_prev_token.start, _prev_token.length);
    return true;
  }
  return false;
}

ValueType Parser::_type_specifier() {
  switch (_curr_token_type) {
  case TokenType::TypeBool:
    _advance();
    return ValueType::Bool;
  case TokenType::TypeInt:
    _advance();
    return ValueType::Int;
  case TokenType::TypeNum:
    _advance();
    return ValueType::Num;
  case TokenType::TypeStr:
    _advance();
    return ValueType::Str;
  default:
    return ValueType::Void;
  }
}

void Parser::_declaration() {
  _last_stmt_returned = false;
  std::string identifier;
  if (_match_id(identifier)) {
    if (_match(TokenType::Colon)) {
      ValueType type_spec = _type_specifier();
      _var_declaration(identifier, type_spec);
    } else {
      _id_stmt();
    }
  } else {
    _stmt();
  }

  if (_panic_mode)
    _synchronize();
}

void Parser::_var_declaration(std::string &identifier, ValueType type) {
  _scope_stack.AddVar(identifier, type);
  ValueType expr_type = _expr();
  _type_error(type, expr_type, expect_var_declaration_type);
  _consume(TokenType::SemiColon, "Expect ';'.");
}

void Parser::_global_declaration() {
  BindingId id;
  if (!_match_id(id)) {
    _error_at_current("Expect identifier.");
  }
  _consume(TokenType::Colon, "Expect ':'.");

  // parse params: id : n int : m num...
  std::vector<Param> params;
  BindingId param_id;
  while (_match_id(param_id)) {
    ValueType type = _type_specifier();
    if (type == ValueType::Void) {
      _error_at_current("Param cannot be void.");
      break;
    }
    params.emplace_back(Param{param_id, type});
    _consume(TokenType::Colon, "Expect ':'.");
  }

  // parse binding type
  if (_match(TokenType::Fn)) {
    _fn_declaration(id, params);
  } else if (ValueType type = _type_specifier(); type != ValueType::Void) {
    _var_declaration(id, type);
  }

  if (_panic_mode)
    _synchronize();
}

void Parser::_fn_declaration(std::string &identifier,
                             const std::vector<Param> &params) {
  // denote end of previous global scope
  _emit_byte(OpCode::Global);

  _return_type = _type_specifier();
  const Int fn_ptr = _byte_code->Length();
  _byte_code->AddFunction(identifier, fn_ptr);

  const uint8_t constant = _make_constant(fn_ptr);
  if (!_scope_stack.AddFn(identifier, constant, params, _return_type)) {
    _error(identifier_bound);
  }

  _scope_stack.PushFnScope();
  for (const auto &param : params) {
    _scope_stack.AddVar(param.id, param.type);
  }

  _consume(TokenType::OpenBrace, "Expect '{'.");
  while (!_match(TokenType::CloseBrace)) {
    _declaration();
  }

  if (!_last_stmt_returned) {
    if (_return_type == ValueType::Void) {
      _emit_byte(OpCode::StackUp);
      _emit_byte(OpCode::Return);
    } else {
      _error("Non-void return type must have explicit return.");
    }
  }
  _scope_stack.Pop();

  // denote start of next global scope
  _emit_byte(OpCode::Global);
}

void Parser::_type_declaration() {}

void Parser::_cmpt_declaration() {}

void Parser::_stmt() {
  if (_match(TokenType::Log)) {
    _log_stmt();
  } else if (_match(TokenType::Return)) {
    _return_stmt();
  } else {
    _expr_stmt();
  }
}

void Parser::_expr_stmt() {
  _expr();
  _consume(TokenType::SemiColon, expect_statment_semi_colon);
  _emit_byte(OpCode::Pop);
}

void Parser::_id_stmt() {
  _identifier();
  _consume(TokenType::SemiColon, expect_statment_semi_colon);
  _emit_byte(OpCode::Pop);
}

void Parser::_log_stmt() {
  ValueType expr_type = _expr();
  if (expr_type == ValueType::Void) {
    _error("Cannot print void value.");
  }
  _consume(TokenType::SemiColon, expect_statment_semi_colon);
  _emit_byte(OpCode::TypeArg);
  _emit_byte(expr_type);
  _emit_byte(OpCode::Log);
}

void Parser::_return_stmt() {
  if (_return_type == ValueType::Void) {
    _emit_byte(OpCode::StackUp);
  } else {
    ValueType expr_type = _expr();
    _type_error(_return_type, expr_type, expect_return_expr_type);
  }
  _consume(TokenType::SemiColon, expect_statment_semi_colon);
  _emit_byte(OpCode::Return);
  _last_stmt_returned = true;
}

ValueType Parser::_expr() { return _parse_precedence(Precedence::Assignment); }

ValueType Parser::_grouping() {
  ValueType type = _expr();
  _consume(TokenType::CloseParen, "Expect ')' after expression.");
  return type;
}

ValueType Parser::_identifier() {
  bool is_global = false;
  std::string identifier(_prev_token.start, _prev_token.length);
  const FnBinding *fn_binding = _scope_stack.FindFn(identifier);
  if (fn_binding != nullptr) {
    _emit_byte(OpCode::StackUp);
    for (const auto &param : fn_binding->get_params()) {
      ValueType type = _expr();
      _type_error(param.type, type, expect_fn_arg_type);
    }
    _emit_byte(OpCode::Constant);
    _emit_byte(fn_binding->get_constant());
    _emit_byte(OpCode::Call);
    _emit_byte(fn_binding->get_params().size());
    return fn_binding->get_return_type();
  }

  const VarBinding *var_binding = _scope_stack.FindVar(identifier, is_global);
  if (var_binding != nullptr) {
    if (_match(TokenType::Equal)) {
      ValueType expr_type = _expr();
      _type_error(var_binding->get_type(), expr_type,
                  expect_var_assignment_type);
      _emit_byte(is_global ? OpCode::SetGlobal : OpCode::SetLocal);
    } else {
      _emit_byte(is_global ? OpCode::GetGlobal : OpCode::GetLocal);
    }
    _emit_byte(var_binding->get_offset());
    return var_binding->get_type();
  }

  _error(identifier_not_bound);
  return ValueType::Invalid;
}

ValueType Parser::_literal() {
  switch (_prev_token_type) {
  case TokenType::True:
    _emit_byte(OpCode::True);
    return ValueType::Bool;
  case TokenType::False:
    _emit_byte(OpCode::False);
    return ValueType::Bool;
  }
}

ValueType Parser::_number() {
  Value value = strtod(_prev_token.start, nullptr);
  _emit_constant(value);

  return ValueType::Num;
}

ValueType Parser::_string() {
  _emit_constant(new Str(_prev_token.start + 1, _prev_token.length - 2));

  return ValueType::Str;
}

ValueType Parser::_unary() {
  TokenType op_token_type = _prev_token_type;
  ValueType type = _parse_precedence(Precedence::Unary);

  switch (op_token_type) {
  case TokenType::Minus:
    _type_error(ValueType::Num, type, expect_operand_type);
    _emit_byte(OpCode::Negate);
    return ValueType::Num;
  case TokenType::Bang:
    _type_error(ValueType::Bool, type, expect_operand_type);
    _emit_byte(OpCode::Not);
    return ValueType::Bool;
  }
}

void Parser::_emit_byte(uint8_t byte) {
  GUISE_ASSERT(_byte_code != nullptr)

  _byte_code->Write(byte);
}

void Parser::_emit_constant(Value value) {
  _emit_byte(OpCode::Constant);
  _emit_byte(_make_constant(value));
}

uint8_t Parser::_make_constant(Value value) {
  GUISE_ASSERT(_byte_code != nullptr)

  int constant = _byte_code->AddConstant(value);
  if (constant > std::numeric_limits<uint8_t>::max()) {
    _error("Too many constants in one chunk.");
    return 0;
  }

  return static_cast<uint8_t>(constant);
}

void Parser::_error_at(TokenType token_type, const Token &token,
                       const char *message) {
  if (_panic_mode)
    return;
  _panic_mode = true;
  fprintf(stderr, "[line %d] Error", token.line);

  if (token_type == TokenType::Eof) {
    fprintf(stderr, " at end");
  } else if (token_type == TokenType::Error) {
    // Nothing.
  } else {
    fprintf(stderr, " at '%.*s'", token.length, token.start);
  }

  fprintf(stderr, ": %s\n", message);
  _had_error = true;
}

void Parser::_error_at_current(const char *message) {
  _error_at(_curr_token_type, _curr_token, message);
}

void Parser::_error(const char *message) {
  _error_at(_prev_token_type, _prev_token, message);
}

void Parser::_synchronize() {
  _panic_mode = false;
  while (_curr_token_type != TokenType::Eof) {
    if (_prev_token_type == TokenType::SemiColon)
      return;
    if (_prev_token_type == TokenType::CloseBrace)
      return;
    _advance();
  }
}
