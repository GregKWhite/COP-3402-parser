#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "constants.h"
#include "structs.h"
#include "lexer.h"
#include "lexer.c"
#include "parser.h"
#include "parser.c"

int main() {
  generateLexemes();
  parse();
  return 0; 
}
