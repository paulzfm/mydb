SOURCES 	= 	$(wildcard *.cpp parser/*.cpp query/*.cpp record/*.cpp sys/*.cpp util/*.cpp parser/lexer.cpp parser/parser.cpp)
OBJECTS 	= 	$(SOURCES:%.cpp=%.o)
LEX			=	flex
YACC		=	bison
CXXFLAGS  	= 	-std=c++11
LDFLAGS  	=
TARGET  	= 	mydb

all: parser/lexer.cpp parser/parser.cpp $(TARGET)

$(TARGET): $(OBJECTS) parser/lexer.o parser/parser.o
	$(CXX) $(LDFLAGS) $(OBJECTS) parser/lexer.o parser/parser.o -o $(TARGET)

parser/lexer.o: parser/lexer.cpp
	$(CXX) -c $(CXXFLAGS) parser/lexer.cpp -o parser/lexer.o

parser/parser.o: parser/parser.cpp
	$(CXX) -c $(CXXFLAGS) parser/parser.cpp -o parser/parser.o

parser/lexer.cpp: parser/lexer.l
	$(LEX) -o parser/lexer.cpp parser/lexer.l

parser/parser.cpp: parser/parser.ypp
	$(YACC) -o parser/parser.cpp -d parser/parser.ypp

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@

clean:
	rm -rf *~ $(TARGET) $(OBJECTS) parser/*.hh parser/parser.cpp parser/parser.hpp parser/lexer.cpp
