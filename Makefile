.PHONY: lex yacc
	echo 'generating lexer and parser.'

##queen: ROBDD to solve n-queen problem
queen: parser.cpp tokens.cpp
	g++ -D QUEEN -O3 -o queen.out prop.cpp tokens.cpp parser.cpp table.cpp

##cmdin: generates ROBDD-parser.
cmdin: parser.cpp tokens.cpp
	g++ -D STDIN -o cmdin.out prop.cpp tokens.cpp parser.cpp table.cpp

##debug: generates ROBDD-parser in debug mode.
debug: parser.cpp tokens.cpp
	g++ -D DEBUG_MODE -o deb.out prop.cpp tokens.cpp parser.cpp table.cpp

##debug2: generates ROBDD-parser in debug2 mode.
debug2: parser.cpp tokens.cpp
	g++ -D DEBUG_MODE2 -o deb2.out prop.cpp tokens.cpp parser.cpp table.cpp

##tokens.cpp: generates lexicon.
tokens.cpp: 
	lex -o tokens.cpp robdd.lex 

##parser.cpp: generates parser.
parser.cpp: 
	bison -d -o parser.cpp robdd.yacc

#clean: removes all generated files from the source.
clean:
	rm -rf *.out parser.* tokens.cpp

##help: options doc
help : Makefile
	@sed -n 's/^##//p' $<
