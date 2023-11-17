#include "scanner.h"

namespace {
// Error strings
const char *error_unexpected_character = "Unexpected character.";
const char *error_unterminated_string = "Unterminated string.";

bool is_digit(char c) { return c >= '0' && c <= '9'; }

bool is_alpha(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}
} // namespace

using namespace GuiSE;

Scanner::Scanner(const char *source)
    : _source(source), _start(_source), _current(_source) {}

TokenType Scanner::ScanToken(Token &token) {
  _skip_whitespace();
  _start = _current;

  if (_is_at_end()) {
    _make_token(token);
    return TokenType::Eof;
  }

  char c = _advance();
  if (is_alpha(c)) {
    return _identifier(token);
  } else if (is_digit(c)) {
    _number(token);
    return TokenType::Number;
  } else if (c == '"') {
    return _string(token);
  }

  _make_token(token);
  switch (c) {
  case '(':
    return TokenType::OpenParen;
  case ')':
    return TokenType::CloseParen;
  case '{':
    return TokenType::OpenBrace;
  case '}':
    return TokenType::CloseBrace;
  case '[':
    return TokenType::OpenBracket;
  case ']':
    return TokenType::CloseBracket;
  case '.':
    return TokenType::Dot;
  case '-':
    return TokenType::Minus;
  case '+':
    return TokenType::Plus;
  case '/':
    return TokenType::Slash;
  case '*':
    return TokenType::Star;
  case ',':
    return TokenType::Comma;
  case ':':
    return TokenType::Colon;
  case ';':
    return TokenType::SemiColon;
  case '!':
    return _match('=') ? TokenType::BangEqual : TokenType::Bang;
  case '=':
    return _match('=') ? TokenType::EqualEqual : TokenType::Equal;
  case '<':
    return _match('=') ? TokenType::LessEqual : TokenType::Less;
  case '>':
    return _match('=') ? TokenType::GreaterEqual : TokenType::Greater;
  }

  _error_token(error_unexpected_character, token);
  return TokenType::Error;
}

char Scanner::_advance() { return *_current++; }

void Scanner::_error_token(const char *error, Token &token) {
  token.start = error;
  token.length = strlen(error);
  token.line = _line;
}

bool Scanner::_is_at_end() { return *_current == '\0'; }

void Scanner::_make_token(Token &token) {
  token.start = _start;
  token.length = _current - _start;
  token.line = _line;
}

void Scanner::_number(Token &token) {
  while (is_digit(_peek()))
    _advance();

  if (_peek() == '.' && is_digit(_peek_next())) {
    _advance();

    while (is_digit(_peek()))
      _advance();
  }

  _make_token(token);
}

bool Scanner::_match(char expected) {
  if (_is_at_end())
    return false;
  if (*_current != expected)
    return false;
  _current++;
  return true;
}

char Scanner::_peek() { return *_current; }

char Scanner::_peek_next() { return _current[1]; }

void Scanner::_skip_whitespace() {
  for (;;) {
    char c = _peek();
    switch (c) {
    case ' ':
    case '\r':
    case '\t':
      _advance();
      break;
    case '\n':
      _line++;
      _advance();
      break;
    case '#':
      while (_peek() != '\n' && !_is_at_end())
        _advance();
      break;
    default:
      return;
    }
  }
}

TokenType Scanner::_string(Token &token) {
  while (!_is_at_end() && _peek() != '"') {
    if (_peek() == '\n') {
      _error_token(error_unterminated_string, token);
      return TokenType::Error;
    }
    _advance();
  }

  if (_is_at_end()) {
    _error_token(error_unterminated_string, token);
    return TokenType::Error;
  }

  _advance();
  _make_token(token);
  return TokenType::String;
}

TokenType Scanner::_identifier(Token &token) {
  while (is_alpha(_peek()) || is_digit(_peek()))
    _advance();
  _make_token(token);
  return _identifier_t();
}

TokenType Scanner::_identifier_t() {
  switch (_start[0]) {
  case 'a':
    return _check_keyword(1, 2, "nd", TokenType::And);
  case 'b':
    return _check_keyword(1, 3, "ool", TokenType::BoolType);
  case 'c':
    return _check_keyword(1, 3, "mpt", TokenType::Cmpt);
  case 'e':
    return _check_keyword(1, 3, "lse", TokenType::Else);
  case 'f':
    if (_current - _start > 1) {
      switch (_start[1]) {
      case 'a':
        return _check_keyword(2, 3, "lse", TokenType::False);
      case 'o':
        return _check_keyword(2, 1, "r", TokenType::For);
      case 'n':
        return _check_keyword(2, 0, "", TokenType::Fn);
      }
    }
    break;
  case 'i':
    return _check_keyword(1, 1, "f", TokenType::If);
  case 'l':
    return _check_keyword(1, 2, "og", TokenType::Log);
  case 'n':
    return _check_keyword(1, 5, "umber", TokenType::NumberType);
  case 'o':
    return _check_keyword(1, 1, "r", TokenType::Or);
  case 's':
    return _check_keyword(1, 2, "tr", TokenType::StrType);
  case 'r':
    return _check_keyword(1, 5, "eturn", TokenType::Return);
  case 't':
    if (_current - _start > 1) {
      switch (_start[1]) {
      case 'r':
        return _check_keyword(2, 2, "ue", TokenType::True);
      case 'y':
        return _check_keyword(2, 2, "pe", TokenType::Type);
      }
    }
    break;
  case 'w':
    return _check_keyword(1, 4, "hile", TokenType::While);
  }
  return TokenType::Identifier;
}

TokenType Scanner::_check_keyword(int start, int length, const char *rest,
                                  TokenType type) {
  if (_current - _start == start + length &&
      memcmp(_start + start, rest, length) == 0) {
    return type;
  }

  return TokenType::Identifier;
}
