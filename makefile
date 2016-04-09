HEADERS = compiler.h

default: program

program: 
	gcc compiler.c -o compiler

clean:
	rm -f compiler
