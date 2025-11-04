#include "parser.h"

bool Parser::match(Token::Type ttype) {
  if (check(ttype)) {
    advance();
    return true;
  }
  return false;
}

bool Parser::check(Token::Type ttype) {
  if (isAtEnd())
    return false;
  return current->type == ttype;
}

bool Parser::advance() {
  if (!isAtEnd()) {
    Token *temp = current;
    if (previous)
      delete previous;
    current = scanner->nextToken();
    previous = temp;
    if (check(Token::ERR)) {
      cout << "Error de análisis, carácter no reconocido: " << current->text
           << endl;
      exit(1);
    }
    return true;
  }
  return false;
}

bool Parser::isAtEnd() { return (current->type == Token::END); }

Parser::Parser(Scanner *sc) : scanner(sc) {
  previous = NULL;
  current = scanner->nextToken();
  if (current->type == Token::ERR) {
    cout << "Error en el primer token: " << current->text << endl;
    exit(1);
  }
}

node *Parser::parse() {
  if (!match(Token::LB)) {
    cout << "Error en el { de inicio: " << current->text << endl;
    exit(0);
  }
  if (!match(Token::OP)) {
    cout << "Se esperaba op \" " << current->text << endl;
    exit(0);
  }

  if (!match(Token::DP)) {
    cout << "Se esperaba : despues del op " << current->text << endl;
    exit(0);
  }

  // union: 0
  // intersection: 1
  // diff: 2
  int op = -1;
  if (match(Token::UNION)) {
    op = 0;
  } else if (match(Token::INTERSECTION)) {
    op = 1;
  } else if (match(Token::DIFF)) {
    op = 2;
  } else {
    match(Token::ID);
  }

  if (!match(Token::COMMA)) {
    cout << "Se esperaba , despues de operador " << current->text << endl;
    exit(0);
  }

  if (!match(Token::FUNCTION)) {
    cout << "Se esperaba function " << current->text << endl;
    exit(0);
  }
  if (!match(Token::DP)) {
    cout << "Se esperaba : despues del function " << current->text << endl;
    exit(0);
  }

  if (op == -1) {
    // we are terminal
    match(Token::ID);
    string fun = previous->text;
    node *temp_node = new node();
    temp_node->function = fun;
    temp_node->op = op;

    if (!match(Token::COMMA)) {
      cout << "Se esperaba , despues de function " << current->text << endl;
      exit(0);
    }
    if (!match(Token::CHILDS)) {
      cout << "Se esperaba dhildren " << current->text << endl;
      exit(0);
    }
    if (!match(Token::DP)) {
      cout << "Se esperaba : despues de childs " << current->text << endl;
      exit(0);
    }
    if (!match(Token::LBC)) {
      cout << "Se esperaba [ despues de childs " << current->text << endl;
      exit(0);
    }
    if (!match(Token::RBC)) {
      cout << "Se esperaba ] despues de childs " << current->text << endl;
      exit(0);
    }
    if (!match(Token::RB)) {
      cout << "Se esperaba } al final " << current->text << endl;
      exit(0);
    }
    return temp_node;
  } else {
    // we have children
    if (!match(Token::ID)) {
      cout << "Se esperaba , al final del function " << current->text << endl;
      exit(0);
    }
    if (!match(Token::COMMA)) {
      cout << "Se esperaba , al final del function " << current->text << endl;
      exit(0);
    }
    if (!match(Token::CHILDS)) {
      cout << "Se esperaba childs " << current->text << endl;
      exit(0);
    }
    if (!match(Token::DP)) {
      cout << "Se esperaba : despues de childs " << current->text << endl;
      exit(0);
    }
    if (!match(Token::LBC)) {
      cout << "Se esperaba [ despues de childs " << current->text << endl;
      exit(0);
    }

    node *father = new node(op);
    father->function = "null";

    while (true) {
      node *children = parse();
      father->children.push_back(children);

      if (match(Token::RBC)) {
        break;
      } else {
        match(Token::COMMA);
      }
    }

    if (!match(Token::RB)) {
      cout << "Se esperaba } al final " << current->text << endl;
      exit(0);
    }

    return father;
  }
}
