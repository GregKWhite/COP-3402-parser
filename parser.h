#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SYMBOL_TABLE_SIZE 100
#define MAX_IDENT_LEN 11
#define MAX_INSTRUCTIONS 2000

typedef struct {
    int type;
    char val[MAX_IDENT_LEN + 1];
} Token;

typedef enum {
    nulsym = 1, identsym, numbersym, plussym, minussym,
    multsym,  slashsym, oddsym, eqsym, neqsym, lessym, leqsym,
    gtrsym, geqsym, lparentsym, rparentsym, commasym, semicolonsym,
    periodsym, becomessym, beginsym, endsym, ifsym, thensym,
    whilesym, dosym, callsym, constsym, varsym, procsym, writesym,
    readsym , elsesym, unknownsym
} token_type;

typedef enum {
    consttype = 1, vartype, proctype
} symbol_type;

typedef struct {
    int kind;      // const = 1, var = 2, proc = 3
    char name[MAX_IDENT_LEN+1]; // name up to 11 chars
    int val;       // number (ASCII value)
    int level;     // L level
    int addr;      // M address
} Symbol;

// Represents each line of input
typedef struct {
    int op; // opcode
    int l; // lexicographical level
    int m; // a number, program address, data address, or ID for OPR 2
} Instruction;

typedef enum {
    LIT = 1, OPR, LOD, STO, CAL, INC, JMP, JPC, SIO
} op_code;

typedef enum {
    RTN = 0, NEG, ADD, SUB, MUL, DIV, ODD, MOD, EQL, NEQ, LSS, LEQ, GTR, GEQ
} m_code;

char* errorCodes[] = {
  "0. I was lazy and didn't do it yet.",
  "1. Use = instead of :=.",
  "2. = must be followed by a number.",
  "3. Identifier must be followed by =.",
  "4. const, var, procedure must be followed by identifier.",
  "5. Semicolon or comma missing.",
  "6. Incorrect symbol after procedure declaration.",
  "7. Statement expected.",
  "8. Incorrect symbol after statement part in block.",
  "9. Period expected.",
  "10. Semicolon between statements missing.",
  "11. Undeclared identifier.",
  "12. Assignment to constant or procedure is not allowed.",
  "13. Assignment operator expected.",
  "14. Call must be followed by an identifier.",
  "15. Call of a constant or variable is meaningless.",
  "16. then expected.",
  "17. Semicolon or } expected.",
  "18. do expected.",
  "19. Incorrect symbol following statement.",
  "20. Relational operator expected.",
  "21. Expression must not contain a procedure identifier.",
  "22. Right parenthesis missing.",
  "23. The preceding factor cannot begin with this symbol.",
  "24. An expression cannot begin with this symbol.",
  "25. This number is too large.",
};

void readTokens();
void getToken();
void program();
void constant();
int variable();
void procedure();
void block();
void statement();
void expression();
void condition();
void term();
void factor();
int relation();

void generate(int op, int l, int m);
int findInTable(char *ident);
void insertConst(char* ident, char* val);
void insertVar(char* ident, int level, int addr);
void insertProc(char *ident); 
void insertSym(Symbol* sym);
void printInstructions();
void error(int code);

void debug();

#endif
