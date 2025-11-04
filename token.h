#ifndef TOKEN_H
#define TOKEN_H

#include <string>

class Token {
public:
  enum Type {
    QM,
    LB,
    RB,
    ID,
    DP,
    LBC,
    RBC,
    COMMA,
    END,
    ERR,
    UNION,
    DIFF,
    INTERSECTION,
    OP,
    FUNCTION,
    CHILDS
  };

  Type type;
  std::string text;

  Token(Type type);
  Token(Type type, char c);
  Token(Type type, const std::string &source, int first, int last);

  friend std::ostream &operator<<(std::ostream &outs, const Token &tok);
  friend std::ostream &operator<<(std::ostream &outs, const Token *tok);
};

#endif // TOKEN_H
