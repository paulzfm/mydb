#ifndef PARSER_DRIVER_H_
#define PARSER_DRIVER_H_

#include <string>

#include "parser.tab.hpp"
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
    void scan_begin();
    void scan_end();

    // handling the parser
    void parse(const std::string& f);

    // error handling
    void error(const yy::location& l, const std::string& m);
    void error(const std::string& m);

    // input string
    std::string file;

    // record the ast
    Tree *ast;
};

#endif // PARSER_DRIVER_H_
