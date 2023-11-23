#pragma once

#include <string>

namespace GuiSE {
enum class TokenType {
  // single character
  OpenParen,
  CloseParen,
  OpenBrace,
  CloseBrace,
  OpenBracket,
  CloseBracket,
  Dot,
  Minus,
  Plus,
  Slash,
  Star,
  Comma,
  Colon,
  SemiColon,
  // one or two character tokens
  Bang,
  BangEqual,
  Equal,
  EqualEqual,
  Greater,
  GreaterEqual,
  Less,
  LessEqual,
  // literals
  Number,
  Integer,
  String,
  Identifier,
  // keywords
  And,
  Cmpt,
  Else,
  If,
  False,
  Fn,
  For,
  Log,
  Or,
  Return,
  True,
  Type,
  TypeBool,
  TypeInt,
  TypeStr,
  TypeNum,
  While,
  // terminating tokens
  Error,
  Eof,
  Count,
  Invalid,
};

struct Token {
  const char *start = nullptr;
  int length = 0;
  int line = -1;
};

class Scanner {
public:
  Scanner(const char *source);

  TokenType ScanToken(Token &token);

private:
  char _advance();
  bool _is_at_end();
  bool _match(char expected);
  char _peek();
  char _peek_next();

  TokenType _integer_or_number(Token &token);
  TokenType _error_token(const char *error, Token &token);
  TokenType _make_token(TokenType token_t, Token &token);
  TokenType _string(Token &token);
  TokenType _identifier(Token &token);
  TokenType _identifier_t();
  TokenType _check_keyword(int start, int length, const char *rest,
                           TokenType type);

  void _skip_whitespace();

  const char *_source;
  const char *_start;
  const char *_current;
  int _line = 1;
};
} // namespace GuiSE