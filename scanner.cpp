#include "scanner.h"
#include "token.h"
#include <cstring>
#include <iostream>

using namespace std;

Scanner::Scanner(const char *s) : input(s), first(0), current(0) {}

bool is_white_space(char c) {
  return c == ' ' || c == '\n' || c == '\r' || c == '\t';
}

Token *Scanner::nextToken() {
  Token *token;
  while (current < input.length() && is_white_space(input[current]))
    current++;
  if (current >= input.length())
    return new Token(Token::END);
  char c = input[current];
  first = current;

  if (c == '\"') {
    current++;
    // (isalnum(input[current]) || strchr("()-^+", c))
    while (current < input.length() && input[current] != '\"')
      current++;

    first++;
    string word = input.substr(first, current - first);
    if (word == "union") {
      token = new Token(Token::UNION, word, 0, word.length());
    } else if (word == "diff") {
      token = new Token(Token::DIFF, word, 0, word.length());
    } else if (word == "intersection") {
      token = new Token(Token::INTERSECTION, word, 0, word.length());
    } else if (word == "op") {
      token = new Token(Token::OP, word, 0, word.length());
    } else if (word == "function") {
      token = new Token(Token::FUNCTION, word, 0, word.length());
    } else if (word == "childs") {
      token = new Token(Token::CHILDS, word, 0, word.length());
    } else {
      token = new Token(Token::ID, word, 0, word.length());
    }

    current++;

  }

  else if (strchr("[]{}\":,", c)) {
    switch (c) {
    case ':':
      token = new Token(Token::DP, c);
      break;
    case '{':
      token = new Token(Token::LB, c);
      break;
    case '}':
      token = new Token(Token::RB, c);
      break;
    case '[':
      token = new Token(Token::LBC, c);
      break;
    case ']':
      token = new Token(Token::RBC, c);
      break;
    case ',':
      token = new Token(Token::COMMA, c);
      break;
    default:
      cout << "No debería llegar acá" << endl;
      token = new Token(Token::ERR, c);
    }
    current++;
  } else {
    token = new Token(Token::ERR, c);
    current++;
  }
  return token;
}

void Scanner::reset() {
  first = 0;
  current = 0;
}

Scanner::~Scanner() {}

void test_scanner(Scanner *scanner) {
  Token *current;
  cout << "Iniciando Scanner:" << endl << endl;
  while ((current = scanner->nextToken())->type != Token::END) {
    if (current->type == Token::ERR) {
      cout << "Error en scanner - carácter inválido: " << current->text << endl;
      break;
    } else {
      cout << *current << endl;
    }
    delete current;
  }
  cout << "TOKEN(END)" << endl;
  delete current;
}
