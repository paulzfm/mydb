SOURCES 	= 	$(wildcard *.cpp engine/*.cpp parser/*.cpp query/*.cpp record/*.cpp sys/*.cpp util/*.cpp)
OBJECTS 	= 	$(SOURCES:%.cpp=%.o)
LEX			=	flex
YACC		=	bison
CXXFLAGS  	= 	-std=c++11
LDFLAGS  	=
TARGET  	= 	mydb

all: parser/lexer.cc parser/parser.cc $(TARGET)

$(TARGET): $(OBJECTS) parser/parser.o parser/lexer.o
	$(CXX) $(LDFLAGS) $(OBJECTS) parser/parser.o parser/lexer.o -o $(TARGET)

parser/lexer.o: parser/lexer.cc
	$(CXX) -c $(CXXFLAGS) parser/lexer.cc -o parser/lexer.o

parser/parser.o: parser/parser.cc
	$(CXX) -c $(CXXFLAGS) parser/parser.cc -o parser/parser.o

parser/lexer.cc: parser/lexer.l
	$(LEX) -o parser/lexer.cc parser/lexer.l

parser/parser.cc: parser/parser.ypp
	$(YACC) -o parser/parser.cc -d parser/parser.ypp

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@

clean:
	rm -rf *~ $(TARGET) $(OBJECTS) parser/*.hh parser/*.cc
