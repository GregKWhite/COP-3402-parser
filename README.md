# COP-3402-parser
Parser for COP 3402 Spring 2016

##### TODO
* Thoroughly test this.
* Write the documentation PDF

### Instructions
To use the code in this repo:
* `$ git pull git@github.com:GregKWhite/COP-3402-parser.git`
* `$ cd COP-3402-parser`

To remove leftover files from the previous run and rebuild the parser:
* `$ make clean ; make`

To run the parser:
* `$ ./parser`
  + The program will print the contents of `lexemelist.txt` as required.
  + If the program is correct, it will print out "No errors, program is
    syntactically correct."
  + If it is not, it will throw an appropriate error, telling you the
    syntactical error.

To check the contents of `symlist.txt`:
* `$ cat symlist.txt`
