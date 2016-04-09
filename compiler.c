#include "compiler.h"
#include "lexer.c"
#include "parser.c"

int main() {
  generateLexemes();
  parse();
  return 0; 
}
