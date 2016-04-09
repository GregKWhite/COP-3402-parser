# COP-3402-parser
Parser for COP 3402 Spring 2016

### Current State
It doesn't work with more complicated examples, so it's probably worth running
this without the lexer. I'll look into it later to make sure the correct tokens
are being generated when I get back in town. 

I'd run it with your own `lexemelist.txt`. Just comment out the line that says
`generateLexemes();` in `compiler.c`. 

If you have any questions, just shoot me a text and I'll answer what I can!

##### TODO
* Implement namespacing (variables called outside of scope should throw an
  error)
* Implement the `-l` directive.
* Add additional comments explaining the parsing process
* Thoroughly test this. I'm fairly confident it will handle basic examples, but
  I'm pretty sure it has its fair share of bugs.
* Write the documentation PDF

### Instructions
To use the code in this repo:
* `$ git pull git@github.com:GregKWhite/COP-3402-parser.git`
* `$ cd COP-3402-parser`

To build the parser:
* `$ make`

To run the parser:
* `$ ./compiler`
  + If the program is correct, it will print out "The program is gramatically
    correct."
  + If it is not, it will throw a (hopefully) appropriate error, telling you the
    grammatical error, along with the token it occurred on.
