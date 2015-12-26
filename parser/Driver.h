#ifndef PARSER_DRIVER_H_
#define PARSER_DRIVER_H_

#include <string>

#include "parser.hpp"
#include "Tree.h"

#define YY_DECL					\
  yy::SQLParser::token_type                         \
  yylex (yy::SQLParser::semantic_type* yylval,      \
         yy::SQLParser::location_type* yylloc,      \
         Driver& driver)
YY_DECL;

class Driver
{
public:
    Driver() {}
    virtual ~Driver() {}

    // handling the scanner
    void scan_begin(const char *in);
    void scan_end();

    // handling the parser
    bool parseString(const std::string& str);
    bool parseFile(const std::string& path);

    // error handling
    void error(const yy::location& l, const std::string& m);
    void error(const std::string& m);

    // input string
    std::string input;

    // input file
    std::string file;

    // record the ast
    Tree *ast;
};

#endif // PARSER_DRIVER_H_
