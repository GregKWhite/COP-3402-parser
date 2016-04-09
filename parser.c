#include "compiler.h"

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

void parse() {
  readTokens();
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
  printf("Parsing `block`. Token is %d %s.\n", token->type, token->val);

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
  printf("Finished parsing `block`. Token is %d %s.\n", token->type, token->val);
}

void constant() {
  printf("Parsing `constant`. Token is %d %s.\n", token->type, token->val);

  do {
    getToken();
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

  if (token->type != semicolonsym) {
    error(10);
  }
  getToken();

  printf("Finished parsing `constant`. Token is %d %s.\n", token->type, token->val);
}

int variable() {
  printf("Parsing `variable`. Token is %d %s.\n", token->type, token->val);
  // The number of variables declared.
  // We need this to know how much space to allocate
  // in our INC instruction in block()
  int numVariables = 0;

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
  printf("Finished parsing `variable`. Token is %d %s.\n", token->type, token->val);
}

// I *think* this is done?
void procedure() {
  printf("Parsing `procedure`. Token is %d %s.\n", token->type, token->val);
  getToken();
  if (token->type != identsym) {
    error(4);
  }
  insertProc(token->val);
  getToken();
  if (token->type != semicolonsym) {
    error(6);
  }
  getToken();
  block();
  if (token->type != semicolonsym) {
    error(10);
  }
  getToken();
  printf("Finished parsing `procedure`. Token is %d %s.\n", token->type, token->val);
}

void statement() {
  printf("Parsing `statement`. Token is %d %s.\n", token->type, token->val);
  if (token->type == identsym) {
    int i = findInTable(token->val);
    if (i == 0) {
      error(11);
    }
    Symbol* sym = symbolTable[i];
    if (sym->kind != vartype) {
      error(4);
    }
    getToken();
    if (token->type != becomessym) {
      error(19);
    }
    getToken();
    expression();

    // Store the result in the variable's location offset
    // by the level and its original address
    /* generate(STO, level - sym->level, sym->addr); */
  }
  else if (token->type == callsym) {
    int i = findInTable(token->val);
    if (i == 0) {
      error(11);
    }

    Symbol* sym = symbolTable[i];

    if (sym->kind != proctype) {
      error(15);
    }

    // Call the procedure at the appropriate location
    /* generate(CAL, level - sym->level, sym->addr); */

    getToken();
    if (token->type != identsym) {
      error(0);
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
      error(0);
    }
    getToken();
  }
  else if (token->type == ifsym) {
    getToken();
    condition();
    if (token->type != thensym) {
      error(0);
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
      error(0);
    }

    getToken();
    statement();
  }
  else if (token->type == writesym || token->type == readsym) {
    getToken();
    if (token->type != identsym) {
      error(0);
    }
    getToken();
  }
  printf("Finished parsing `statement`. Token is %d %s.\n", token->type, token->val);
}

void condition() {
  printf("Parsing `condition`. Token is %d %s.\n", token->type, token->val);

  if (token->type == oddsym) {
    getToken();
    expression();
  }
  else {
    expression();
    if (!relation()) {
      error(0);
    }
    getToken();
    expression();
  }
  printf("Finished parsing `condition`. Token is %d %s.\n", token->type, token->val);
}

void expression() {
  printf("Parsing `expression`. Token is %d %s.\n", token->type, token->val);

  term();
  while (token->type == plussym || token->type == minussym) {
    getToken();
    term();
  }
  printf("Finished parsing `expression`. Token is %d %s.\n", token->type, token->val);
}

void term() {
  printf("Parsing `term`. Token is %d %s.\n", token->type, token->val);
  factor();

  while (token->type == multsym || token->type == slashsym) {
    getToken();
    factor();
  }
  printf("Finished parsing `term`. Token is %d %s.\n", token->type, token->val);
}

void factor() {
  printf("Parsing `factor`. Token is %d %s.\n", token->type, token->val);

  if (token->type == identsym) {
    int i = findInTable(token->val);
    if (i == 0) {
      // Error undeclared ident
    }
    Symbol* sym = symbolTable[i];
    if (sym->kind == vartype) {
      /* generate(LOD, sym->level, sym->addr); */
    }
    else if (sym->kind == consttype) {
      /* generate(LIT, sym->level, sym->addr); */
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
      error(0);
    }
    getToken();
  }
  else {
    error(0);
  }
  printf("Finished parsing `factor`. Token is %d %s.\n", token->type, token->val);
}

// Returns 1 if the token's type
// is a relation symbol (> >= < <= = !=).
// 0 otherwise.
int relation() {
  return (token->type >= eqsym && token->type <= geqsym);
}

// Returns the index if a symbol is found in the symbol table.
// 0 otherwise.
//TODO: find a way to implement namespacing
int findInTable(char* ident) {
  int i;
  for (i = 1; i < symbolIndex; i++) {
    if (strcmp(ident, symbolTable[i]->name) == 0) {
      return i;
    }
  }

  return 0;
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
}

void printInstructions() {
  int i;
  for (i = 0; i < instructionIndex; i++) {
    Instruction* inst = instructions[i];
    printf("%d\t%d\t%d\n", inst->op, inst->l, inst->m);
  }
}

void error(int code) {
  fprintf(stderr, "Line %d.\t", tokenIndex); 
  fprintf(stderr, "%s", errorCodes[code]);
  exit(EXIT_FAILURE);
}
