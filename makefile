HEADERS = compiler.h

default: program

program: 
	gcc parser.c -o parser

clean:
	rm -f parser
