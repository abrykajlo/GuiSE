#include "parser.h"

#include <guise/debug.h>
#include <guise/vm/byte_code.h>
#include <guise/vm/object.h>
#include <guise/vm/opcode.h>

using namespace GuiSE;

namespace {
const char *expect_num_operand = "Expect num type operand.";
const char *expect_bool_operand = "Expect bool type operand.";

const char *expect_num_left_operand = "Expect num type left operand.";
const char *expect_num_right_operand = "Expect num type right operand.";

const char *expect_bool_left_operand = "Expect bool type left operand.";
const char *expect_bool_right_operand = "Expect bool type right operand.";

const char* semi_colon_after_statment = "Expect ';' after statement.";

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
    _error(message);
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

  const char *expect_left_message = nullptr;
  const char *expect_right_message = nullptr;
  if (expected_type == ValueType::Bool) {
    expect_left_message = expect_bool_left_operand;
    expect_right_message = expect_bool_right_operand;
  } else if (expected_type == ValueType::Num) {
    expect_left_message = expect_num_left_operand;
    expect_right_message = expect_num_right_operand;
  }
  _type_error(expected_type, left_type, expect_left_message);
  _type_error(expected_type, right_type, expect_right_message);

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

bool Parser::_match_identifier(std::string &identifier) {
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
  std::string identifier;
  if (_match_identifier(identifier)) {
    if (_match(TokenType::Colon)) {
      ValueType type_spec = _type_specifier();
      _var_declaration(identifier, type_spec);
    }
    else if (_match(TokenType::Equal)) {
        bool is_global = false;
        const VarBinding* var_binding = _scope_stack.FindVar(identifier, is_global);
        if (var_binding != nullptr) {
            ValueType expr_type = _expr();
            _type_error(var_binding->get_type(), expr_type, "Can't assign type to variable.");
            _emit_byte(is_global ? OpCode::SetGlobal : OpCode::SetLocal);
            _emit_byte(var_binding->get_offset());
            _emit_byte(OpCode::Pop);
            _consume(TokenType::SemiColon, semi_colon_after_statment);
        }
        else {
            _error("Cannot assign to unbound variable.");
        }
    }
  } else {
    _stmt();
  }

  if (_panic_mode)
    _synchronize();
}

void Parser::_var_declaration(std::string &identifier, ValueType type) {
  if (!_scope_stack.AddVar(identifier, type)) {
    _error_at_current(identifier_bound);
  }
  ValueType expr_type = _expr();
  _type_error(type, expr_type,
              "Incorrect expression type when initializing variable.");
  _consume(TokenType::SemiColon, "Expect ';'.");
}

void Parser::_global_declaration() {
  std::string identifier;
  if (!_match_identifier(identifier)) {
    _error_at_current("Expect identifier.");
  }
  _consume(TokenType::Colon, "Expect ':'.");

  // parse params: id : n int : m num...
  std::vector<Param> params;

  // parse binding type
  if (_match(TokenType::Fn)) {
    _fn_declaration(identifier, params);
  } else if (ValueType type = _type_specifier(); type != ValueType::Void) {
    _var_declaration(identifier, type);
  }

  if (_panic_mode)
    _synchronize();
}

void Parser::_fn_declaration(std::string &identifier,
                             std::vector<Param> &params) {
  _emit_byte(OpCode::Global); // denote end of previous global scope
  const Int fn_ptr = _byte_code->Length();
  const uint8_t constant = _make_constant(fn_ptr);
  if (!_scope_stack.AddFn(identifier, constant, params, ValueType::Void)) {
    _error(identifier_bound);
  }
  _scope_stack.PushFnScope();
  _byte_code->AddFunction(identifier, fn_ptr);
  _consume(TokenType::OpenBrace, "Expect '{'.");
  while (!_match(TokenType::CloseBrace)) {
    _declaration();
  }
  _emit_byte(OpCode::Return);
  _scope_stack.Pop();
  _emit_byte(OpCode::Global); // denote start of next global scope
}

void Parser::_type_declaration() {}

void Parser::_cmpt_declaration() {}

void Parser::_stmt() {
  if (_match(TokenType::Log)) {
    _log_stmt();
  }
  else {
      _expr_stmt();
  }
}

void Parser::_expr_stmt()
{
    _expr();
    _emit_byte(OpCode::Pop);
    _consume(TokenType::SemiColon, semi_colon_after_statment);
}

void Parser::_log_stmt() {
  ValueType print_type = _expr();
  if (print_type == ValueType::Void) {
    _error("Cannot print void value.");
  }
  _consume(TokenType::SemiColon, semi_colon_after_statment);
  _emit_byte(OpCode::TypeArg);
  _emit_byte(print_type);
  _emit_byte(OpCode::Log);
}

ValueType Parser::_expr() {
  return _parse_precedence(Precedence::Assignment);
}

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
    return fn_binding->get_return_type();
  }

  const VarBinding *var_binding = _scope_stack.FindVar(identifier, is_global);
  if (var_binding != nullptr) {
    if (_match(TokenType::Equal)) {
       ValueType expr_type = _expr();
       _type_error(var_binding->get_type(), expr_type, "Can't assign type to variable.");
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
    _type_error(ValueType::Num, type, "Expect number type operand.");
    _emit_byte(OpCode::Negate);
    return ValueType::Num;
  case TokenType::Bang:
    _type_error(ValueType::Bool, type, "Expect bool type operand.");
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
