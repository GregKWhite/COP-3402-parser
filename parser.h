#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SYMBOL_TABLE_SIZE 100
#define MAX_IDENT_LEN 11

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
  readsym , elsesym, unknownsym } token_type;

typedef enum {
  consttype = 1, vartype, proctype
} symbol_type;

typedef struct {
  int kind;      // const = 1, var = 2, proc = 3
  char name[12]; // name up to 11 chars
  int val;       // number (ASCII value)
  int level;     // L level
  int addr;      // M address
} Symbol;

void getToken();
void program();
void block();
void statement();
void expression();
void condition();
void term();
void factor();
int relation();

int generate(int a, int b, int c);
int findInTable(char *ident);
void insertConst(char* ident, char* val);
void insertVar(char* ident, int level);
void insertProc(char *ident); 

#endif
