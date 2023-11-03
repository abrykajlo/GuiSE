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

Scanner::Scanner() { _reset(); }

Token Scanner::ScanToken() {
  _skip_whitespace();
  _start = _current;

  if (_is_at_end()) {
    return _make_token(TokenType::Eof);
  }

  char c = _advance();
  if (is_alpha(c))
    return _identifier();
  if (is_digit(c))
    return _number();
  switch (c) {
  case '(':
    return _make_token(TokenType::OpenParen);
  case ')':
    return _make_token(TokenType::CloseParen);
  case '{':
    return _make_token(TokenType::OpenBrace);
  case '}':
    return _make_token(TokenType::CloseBrace);
  case '[':
    return _make_token(TokenType::OpenBracket);
  case ']':
    return _make_token(TokenType::CloseBracket);
  case '.':
    return _make_token(TokenType::Dot);
  case '-':
    return _make_token(TokenType::Minus);
  case '+':
    return _make_token(TokenType::Plus);
  case '/':
    return _make_token(TokenType::Slash);
  case '*':
    return _make_token(TokenType::Star);
  case ',':
    return _make_token(TokenType::Comma);
  case ':':
    return _make_token(TokenType::Colon);
  case ';':
    return _make_token(TokenType::SemiColon);
  case '!':
    return _make_token(_match('=') ? TokenType::BangEqual : TokenType::Bang);
  case '=':
    return _make_token(_match('=') ? TokenType::EqualEqual : TokenType::Equal);
  case '<':
    return _make_token(_match('=') ? TokenType::LessEqual : TokenType::Less);
  case '>':
    return _make_token(_match('=') ? TokenType::GreaterEqual
                                   : TokenType::Greater);
  case '"':
    return _string();
  }

  return _error_token(error_unexpected_character);
}

char Scanner::_advance() { return *_current++; }

Token Scanner::_error_token(const char *error) {
  Token token{
      TokenType::Error,
      error,
      static_cast<int>(strlen(error)),
      _line,
  };
  return token;
}

bool Scanner::_is_at_end() { return _current == &_source[_source.size()]; }

Token Scanner::_make_token(TokenType type) {
  Token token{type, _start, _current - _start, _line};
  return token;
}

Token Scanner::_number() {
  while (is_digit(_peek()))
    _advance();

  if (_peek() == '.' && is_digit(_peek_next())) {
    _advance();

    while (is_digit(_peek()))
      _advance();
  }

  return _make_token(TokenType::Number);
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

char GuiSE::Scanner::_peek_next() { return _current[1]; }

void Scanner::_reset() {
  _start = &_source[0];
  _current = &_source[0];
  _line = 1;
}

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

Token Scanner::_string() {
  while (!_is_at_end() && _peek() != '"') {
    if (_peek() == '\n')
      return _error_token(error_unterminated_string);
    _advance();
  }

  if (_is_at_end())
    return _error_token(error_unterminated_string);

  _advance();
  return _make_token(TokenType::String);
}

Token Scanner::_identifier() {
  while (is_alpha(_peek()) || is_digit(_peek()))
    _advance();
  return _make_token(_identifier_type());
}

TokenType Scanner::_identifier_type() {
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
