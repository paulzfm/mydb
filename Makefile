all: mydb

CXXFLAGS	=	-std=c++11
LEX			=	flex
YACC		=	bison

mydb: parser/Driver.cpp parser/parser.cpp parser/lexer.cpp parser/Tree.cpp main.cpp
	$(CXX) $(CXXFLAGS) -o mydb parser/Driver.cpp parser/parser.cpp parser/lexer.cpp parser/Tree.cpp main.cpp

parser/lexer.cpp: parser/lexer.l
	$(LEX) -o parser/lexer.cpp parser/lexer.l

parser/parser.cpp: parser/parser.ypp
	$(YACC) -o parser/parser.cpp -d parser/parser.ypp

clean:
	rm -rf mydb parser/*.hh parser/parser.cpp parser/parser.hpp parser/lexer.cpp
