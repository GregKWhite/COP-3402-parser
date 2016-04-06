#include "parser.h"

Token* token = NULL;
Symbol* symbolTable[MAX_SYMBOL_TABLE_SIZE];
int symbolIndex = 0;

FILE* input;

int main() {
  input = fopen("lexemelist.txt", "r");
}

void program() {
  getToken();
  block();
}

void block() {
  // Parse constants first.
  if (token->type == constsym) {
    do {
      getToken();
      Token* ident = token;
      if (token->type != identsym) {
        // Throw an error
      }
      getToken();
      if (token->type != eqsym) {
        // Throw an error
      }
      getToken();
      if (token->type != numbersym) {
        // Throw an error
      }
      insertConst(ident->val, token->val);
      getToken();
    } while (token->type == commasym);

    if (token->type != semicolonsym) {
      // Throw an error
    }
    getToken();
  }

  // Parse variables second.
  if (token->type == varsym) {
    do {
      getToken();
      if (token->type != identsym) {
        // Throw an error
      }
      insertVar(token->val, 0); //TODO: Figure out how to insert level
      getToken();
    } while (token->type == commasym);

    if (token->type != semicolonsym) {
      // Throw an error
    }
    getToken();
  }

  // Parse procedures last
  while (token->type == procsym) {
    getToken();
    if (token->type != identsym) {
      // Throw an error
    }
    insertProc(token->val);
    getToken();
    if (token->type != semicolonsym) {
      // Throw an error
    }
    getToken();
    block();
    if (token->type != semicolonsym) {
      // Throw an error
    }
    getToken();
  }
  statement();
}

void statement() {
  if (token->type == identsym) {
    int i = findInTable(token->val);
    if (i == 0) {
      // Error undeclared identifier
    }
    Symbol* sym = symbolTable[i];
    if (sym->kind != vartype) {
      // Error cannot assign to proc or const
    }
    getToken();
    if (token->type != becomessym) {
      // Throw an error
    }
    getToken();
    expression();

    // gen(STO, sym->level, sym->addr);
  }
  else if (token->type == callsym) {
    getToken();
    if (token->type != identsym) {
      // Throw an error
    }
    getToken();
  }
  else if (token->type == beginsym) {
    getToken();
    statement();
    while (token->type == semicolonsym) {
      getToken();
      statement();
    }
    if (token->type != endsym) {
      // Throw an error
    }
    getToken();
  }
  else if (token->type == ifsym) {
    getToken();
    condition();
    if (token->type != thensym) {
      // Throw an error
    }
    getToken();
    statement();
  }
  else if (token->type == whilesym) {
    getToken();
    condition();
    if (token->type != dosym) {
      // Throw an error
    }
    getToken();
    statement();
  }
}

void condition() {
  if (token->type == oddsym) {
    getToken();
    expression();
  }
  else {
    expression();
    if (!relation()) {
      // Throw an error
    }
    getToken();
    expression();
  }
}

void expression() {
  if (token->type == plussym || token->type == minussym) {
    getToken();
  }
  term();
  while (token->type == plussym || token->type == minussym) {
    getToken();
    term();
  }
}

void term() {
  factor();
  while (token->type == multsym || token->type == slashsym) {
    getToken();
    factor();
  }
}

void factor() {
  if (token->type == identsym) {
    int i = findInTable(token->val);
    if (i == 0) {
      // Error undeclared ident
    }
    Symbol* sym = symbolTable[i];
    if (sym->kind == vartype) {
      // gen(LOD, sym->level, sym->addr);
    }
    else if (sym->kind == consttype) {
      // gen(LIT, sym->level, sym->addr);
    }
    else {
      // Error of some sort
    }
    getToken();
  }
  else if (token->type == numbersym) {
    getToken();
  }
  else if (token->type == lparentsym) {
    getToken();
    expression();
    if (token->type != rparentsym) {
      // Throw an error
    }
    getToken();
  }
  else {
    // Throw an error
  }
}

// Returns 1 if the token's type
// is a relation symbol (> >= < <= = !=).
// 0 otherwise.
int relation() {
  return (token->type >= eqsym && token->type <= geqsym);
}

int findInTable(char* ident) {
  return 0;
}

void insertConst(char* ident, char* val) {
  // Create the new symbol from the information given
  Symbol *sym = (Symbol*)(malloc(sizeof(Symbol)));
  sym->kind = consttype;
  strcpy(sym->name, ident);
  sym->val = atoi(val);
}

void insertVar(char *ident, int level) {
  // Create the new symbol from the information given
  Symbol *sym = (Symbol*)(malloc(sizeof(Symbol)));
  sym->kind = vartype;
  strcpy(sym->name, ident);
  sym->level = level;
}

void insertProc(char* ident) {
  // Create the new symbol from the information given
  Symbol *sym = (Symbol*)(malloc(sizeof(Symbol)));
  sym->kind = proctype;
  strcpy(sym->name, ident);
}

void insertSym(Symbol* sym) {
  // Return if the symbol already exists in the table
  if (findInTable(sym->name)) {
    return;
  }

  // Store the symbol in our table
  symbolTable[symbolIndex++] = sym;
}

void getToken() {
  token = malloc(sizeof(Token));
  // Read the next token type from the lexeme list.
  fscanf(input, "%d", &token->type);

  // If the token type is identsym or numbersym,
  // read the next token as well, as we need the
  // value associated with the symbol type
  if (token->type == identsym || token->type == numbersym) {
    fscanf(input, "%s ", token->val);
  }
}
