#include "constants.h"
#include "structs.h"
#include "parser.h"

// The array containing tokens from lexemelist.txt
Token* tokenList[MAX_INSTRUCTIONS];
int tokenCount = 0;
int tokenIndex = 0;

// The current token being examined
Token* token;

Symbol* symbolTable[MAX_SYMBOL_TABLE_SIZE];
int symbolIndex = 1;

// The current block level
int level = -1;

Instruction* instructions[MAX_INSTRUCTIONS];
int instructionIndex = 0;

int main() {
  // Read the tokens from lexemelist.txt into tokenList
  readTokens();

  // Parse the program!
  program();

  // Let the user know the program is grammatically correct.
  printf("The program is gramatically correct.\n");
}

void program() {
  getToken();
  block();

  // If our last token isn't a period,
  // throw an error
  if (token->type != periodsym) {
    error(9);
  }
}

void block() {
  // Increase the level by one
  ++level;

  // The space needed for the return value, SL, DL, and return address
  int space = 4;

  // Parse constants first.
  if (token->type == constsym) {
    constant();
  }

  // Parse variables second.
  if (token->type == varsym) {

    // Make sure to allocate space for each of the variable declarations
    space += variable();
  }

  // Parse procedures last
  while (token->type == procsym) {
    procedure();
  }

  // Parse a statement next, after we're done defining
  // constants, vars, and procedures
  statement();

  // We're done with this block so we're going up a level
  --level;
}

void constant() {

  // Constants are of the form
  // ident = number(, ident = number)*;
  do {
    getToken();
    // Store the identifier so we can insert the constant into the
    // symbol table.
    Token* ident = token;
    if (token->type != identsym) {
      error(4);
    }

    getToken();
    if (token->type != eqsym) {
      error(3);
    }

    getToken();
    if (token->type != numbersym) {
      error(2);
    }

    // Insert the constant into our symbol table
    insertConst(ident->val, token->val);
    getToken();
  } while (token->type == commasym);

  // Constant declarations must end with a semicolon.
  // If one is not found, throw an error.
  if (token->type != semicolonsym) {
    error(10);
  }

  getToken();
}

int variable() {
  // The number of variables declared.
  // We need this to know how much space to allocate
  // in our INC instruction in block()
  int numVariables = 0;

  // Variables are of the form ident(, ident)*;
  do {
    // We're expecting an identifier; if we don't find one
    // we need to throw an error
    getToken();
    if (token->type != identsym) {
      error(4);
    }

    // Insert our variable into the symbol table
    // We use numVariables + 4 to indicate its position
    // in the current AR; starting at 4 since its offset is
    // 4 by default.
    insertVar(token->val, level, numVariables + 4);
    getToken();

    // Create space for each of the variables
    // that need to be stored
    numVariables += 1;
  } while (token->type == commasym);

  // If our variable declarations don't
  // end with a semicolon, throw an error
  if (token->type != semicolonsym) {
    error(10);
  }
  getToken();

  // Return the number of declarations
  // so we can allocate space for them in
  // the block parsing
  return numVariables;
}

void procedure() {
  // If 'procedure' isn't followed by an identifier, throw an error.
  getToken();
  if (token->type != identsym) {
    error(4);
  }

  // Insert our procdure's identifier into the symbol table
  insertProc(token->val);

  // If our procedure declaration isn't followed by a semicolon,
  // throw an error.
  getToken();
  if (token->type != semicolonsym) {
    error(6);
  }

  // After a procedure declaration, a block follows.
  getToken();
  block();

  // If the block is not followed by a semicolon, throw an error
  if (token->type != semicolonsym) {
    error(10);
  }

  getToken();
}

void statement() {
  // If an identifier is found,
  // we're looking for an assignment.
  if (token->type == identsym) {
    // If the identifier is not in our table, throw an error
    Symbol* sym = findInTable(token->val);
    if (!sym) {
      error(11);
    }

    // If the symbol is not a variable
    // (it is a constant or procedure) throw an error
    if (sym->kind != vartype) {
      error(4);
    }

    // If the next token is not :=
    // throw an error.
    getToken();
    if (token->type != becomessym) {
      error(19);
    }

    // Parse the expression that follows :=
    getToken();
    expression();
  }

  // If 'call' is found, we're looking for
  // a procedure identifier
  else if (token->type == callsym) {
    // If the identifier is not in our table, throw an error
    getToken();
    Symbol* sym = findInTable(token->val);
    if (!sym) {
      error(11);
    }

    // If the symbol is not a procedure
    // (it is a constant or variable) throw an error
    if (sym->kind != proctype) {
      error(15);
    }

    if (token->type != identsym) {
      error(14);
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
      error(8);
    }
    getToken();
  }
  else if (token->type == ifsym) {
    getToken();
    condition();
    if (token->type != thensym) {
      error(16);
    }
    getToken();

    statement();

    // else group optional
    if (token->type == elsesym) {
      getToken();
      statement();
    }
  }
  else if (token->type == whilesym) {
    condition();

    if (token->type != dosym) {
      error(18);
    }

    getToken();
    statement();
  }
  else if (token->type == writesym || token->type == readsym) {
    getToken();

    // Read/write must be followed by an identifier.
    if (token->type != identsym) {
      error(0);
    }

    // If the identifier doesn't exist,
    // throw an error
    Symbol *sym = findInTable(token->val);
    if (!sym) {
      // Undeclared identifier found
      error(11);
    }

    getToken();
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
      error(20);
    }
    getToken();
    expression();
  }
}

void expression() {

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
    Symbol *sym = findInTable(token->val);
    if (!sym) {
      error(11);
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
      error(22);
    }
    getToken();
  }
  else {
    printf("Token is %d %s\n", token->type, token->val);
    // 23 i think?
    error(23);
  }
}

// Returns 1 if the token's type
// is a relation symbol (> >= < <= = !=).
// 0 otherwise.
int relation() {
  return (token->type >= eqsym && token->type <= geqsym);
}

Symbol* findInTable(char *ident) {
  int i;
  for (i = 1; i < symbolIndex; i++) {
    if (strcmp(ident, symbolTable[i]->name) == 0) {
      return symbolTable[i];
    }
  }

  return NULL;
}

void insertConst(char* ident, char* val) {
  // Create the new symbol from the information given
  Symbol *sym = (Symbol*)(malloc(sizeof(Symbol)));
  sym->kind = consttype;
  strcpy(sym->name, ident);
  sym->val = atoi(val);

  sym->level = 0;
  sym->addr = 0;

  insertSym(sym);
}

void insertVar(char *ident, int level, int addr) {
  // Create the new symbol from the information given
  Symbol *sym = (Symbol*)(malloc(sizeof(Symbol)));
  sym->kind = vartype;
  strcpy(sym->name, ident);
  sym->level = level;
  sym->addr = addr;

  insertSym(sym);
}

void insertProc(char* ident) {
  // Create the new symbol from the information given
  Symbol *sym = (Symbol*)(malloc(sizeof(Symbol)));
  sym->kind = proctype;
  strcpy(sym->name, ident);

  insertSym(sym);
}

void insertSym(Symbol* sym) {
  // Return if the symbol already exists in the table
  if (findInTable(sym->name)) {
    return;
  }

  // Store the symbol in our table
  symbolTable[symbolIndex++] = sym;
}

// Generates an instruction
void generate(int op, int l, int m) {
  Instruction *inst = (Instruction*)(malloc(sizeof(Instruction)));
  inst->op = op;
  inst->l = l;
  inst->m = m;

  instructions[instructionIndex++] = inst;
}

void readTokens() {
  FILE *input = fopen("lexemelist.txt", "r");
  while (!feof(input)) {
    Token *curToken = (Token*)malloc(sizeof(Token));
    strcpy(curToken->val, "");

    // Read the next token type from the lexeme list.
    fscanf(input, "%d ", &curToken->type);

    // If the token type is identsym or numbersym,
    // read the next token as well, as we need the
    // value associated with the symbol type
    if (curToken->type == identsym || curToken->type == numbersym) {
      fscanf(input, "%[^ \n]s", curToken->val);
    }
    printf("%d.\t%d\t%s\n", tokenCount, curToken->type, curToken->val);

    tokenList[tokenCount++] = curToken;
  }
}

void getToken() {
  token = tokenList[tokenIndex++];

  // If the token is NULL, we just initialize an empty token.
  // This is so we don't get a segfault later on if we read NULL token.
  if (!token) {
    token = (Token*)(malloc(sizeof(Token)));
    strcpy(token->val, "");
    token->type = unknownsym;
  }
}

void error(int code) {
  // fprintf(stderr, "Line %d.\t", tokenIndex);
  fprintf(stderr, "%s", errorCodes[code]);
  exit(EXIT_FAILURE);
}
