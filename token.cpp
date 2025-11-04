#include "token.h"
#include <iostream>

using namespace std;

Token::Token(Type type) : type(type) { text = ""; }

Token::Token(Type type, char c) : type(type) { text = string(1, c); }

Token::Token(Type type, const string &source, int first, int last)
    : type(type) {
  text = source.substr(first, last);
}

std::ostream &operator<<(std::ostream &outs, const Token &tok) {
  switch (tok.type) {
  case Token::QM:
    outs << "TOKEN(QM)";
    break;
  case Token::LB:
    outs << "TOKEN(LB)";
    break;
  case Token::RB:
    outs << "TOKEN(RB)";
    break;
  case Token::ID:
    outs << "TOKEN(ID)";
    break;
  case Token::DP:
    outs << "TOKEN(DP)";
    break;
  case Token::ERR:
    outs << "TOKEN(ERR)";
    break;
  case Token::LBC:
    outs << "TOKEN(LBC)";
    break;
  case Token::RBC:
    outs << "TOKEN(RBC)";
    break;
  case Token::END:
    outs << "TOKEN(END)";
    break;
  case Token::COMMA:
    outs << "TOKEN(COMMA)";
    break;
  case Token::UNION:
    outs << "TOKEN(UNION)";
    break;
  case Token::DIFF:
    outs << "TOKEN(DIFF)";
    break;
  case Token::INTERSECTION:
    outs << "TOKEN(INTERSECTION)";
    break;
  case Token::OP:
    outs << "TOKEN(OP)";
    break;
  case Token::FUNCTION:
    outs << "TOKEN(FUNCTION)";
    break;
  case Token::CHILDS:
    outs << "TOKEN(CHILDS)";
    break;
  default:
    outs << "TOKEN(UNKNOWN)";
    break;
  }
  return outs;
}

std::ostream &operator<<(std::ostream &outs, const Token *tok) {
  return outs << *tok;
}
