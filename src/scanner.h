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
  Or,
  Return,
  True,
  Type,
  While,

  Error,
  Eof
};

struct Token {
  TokenType type;
  const char *start;
  int length;
  int line;
};

class Scanner {
public:
  Scanner();

  Token ScanToken();

  inline void set_source(const std::string &source) {
    _source = source;
    _reset();
  }

private:
  char _advance();
  bool _is_at_end();
  bool _match(char expected);
  char _peek();
  char _peek_next();

  Token _error_token(const char *error);
  Token _make_token(TokenType type);
  Token _number();
  Token _string();
  Token _identifier();
  TokenType _identifier_type();
  TokenType _check_keyword(int start, int length, const char *rest,
                           TokenType type);

  void _reset();
  void _skip_whitespace();

  std::string _source;
  const char *_start = nullptr;
  const char *_current = nullptr;
  int _line = 1;
};
} // namespace GuiSE