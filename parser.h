#ifndef PARSER_H_
#define PARSER_H_

#include "scanner.h"
#include "sphere.h"
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

// 0: union
// 1: intersection
// 2: diff

class node {
public:
  int op;
  string function;
  vector<node *> children;
  node(int o) : op(o) {}
  node() : op(-1) {}
  // evaluates the function. Assumes that the function is valid if op is -1
  float evaluate(double px, double py, double pz) {
    if (op == -1) {
      int center_x_start = function.find("x") + 1;
      int center_x_end = function.find(")", center_x_start);

      int x =
          stoi(function.substr(center_x_start, center_x_end - center_x_start));

      int center_y_start = function.find("y") + 1;
      int center_y_end = function.find(")", center_x_start);
      int y =
          stoi(function.substr(center_y_start, center_y_end - center_y_start));

      int center_z_start = function.find("z") + 1;
      int center_z_end = function.find(")", center_x_start);
      int z =
          stoi(function.substr(center_z_start, center_z_end - center_z_start));

      int radious_start = function.find("-", center_x_end) + 1;
      int radious_end = function.find("^", radious_start);
      int radious =
          stoi(function.substr(radious_start, radious_end - radious_start));
      return Fy_sphere(px, py, pz, x, y, z, radious);
    } else {
      float result = 0;
      if (op == 0) {
        result = children[0]->evaluate(px, py, pz);
        for (int i = 1; i < children.size(); i++) {
          result = min(result, children[i]->evaluate(px, py, pz));
        }
      } else if (op == 1) {
        result = children[0]->evaluate(px, py, pz);
        for (int i = 1; i < children.size(); i++) {
          result = max(result, children[i]->evaluate(px, py, pz));
        }
      } else if (op == 2) {
        result = children[0]->evaluate(px, py, pz);
        for (int i = 1; i < children.size(); i++) {
          result = max(result, -children[i]->evaluate(px, py, pz));
        }
      } else {
        cout << "Invalid operator" << endl;
        exit(1);
      }
      return result;
    }
  }
};

class Parser {
private:
  Scanner *scanner;
  Token *current, *previous;
  bool match(Token::Type ttype);
  bool check(Token::Type ttype);
  bool advance();
  bool isAtEnd();

public:
  Parser(Scanner *scanner);
  node *parse();
};
#endif // PARSER_H_
