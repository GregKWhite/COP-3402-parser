#include "parser.h"

Token* token;

Symbol* symbolTable[MAX_SYMBOL_TABLE_SIZE];
int symbolIndex = 1;

// The current block level
int level = -1;

Instruction* instructions[MAX_INSTRUCTIONS];
int instructionIndex = 0;

FILE* input;

int main() {
  input = fopen("lexemelist.txt", "r");
  return 0;
}
