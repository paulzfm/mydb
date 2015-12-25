%{
#include <cstdlib>
#include <string.h>
#include "Driver.h"
#include "parser.hpp"

#undef yywrap
#define yywrap() 1

#define yyterminate() return token::END
%}

%option noyywrap nounput caseless

%{
#define YY_USER_ACTION  yylloc->columns(yyleng);
%}

%% /* tokens */

%{
    typedef yy::SQLParser::token token;
%}

"("      { return token::LP; }
")"      { return token::RP; }
","      { return token::COMMA; }
";"      { return token::STOP; }

"="      { return token::EQ; }
"<>"     { return token::NE; }
"<="     { return token::LE; }
">="     { return token::GE; }
"<"      { return token::LT; }
">"      { return token::GT; }

"+"      { return token::PLUS; }
"-"      { return token::MINUS; }
"*"      { return token::MUL; }
"/"      { return token::DIV; }
"%"      { return token::MOD; }
"."      { return token::DOT; }

"DATABASE"  { return token::DATABASE;  }
"DATABASES" { return token::DATABASES; }
"TABLE"     { return token::TABLE;     }
"TABLES"    { return token::TABLES;    }
"SHOW"      { return token::SHOW;      }
"CREATE"    { return token::CREATE;    }
"DROP"      { return token::DROP;      }
"USE"       { return token::USE;       }
"CHECK"     { return token::CHECK;     }
"PRIMARY"   { return token::PRIMARY;   }
"KEY"       { return token::KEY;       }
"UNIQUE"    { return token::UNIQUE;    }
"NOT"       { return token::NOT;       }
"NULL"      { return token::NIL;      }
"AUTO_INCREMENT"    { return token::AUTO_INCREMENT; }
"INSERT"    { return token::INSERT;    }
"INTO"      { return token::INTO;      }
"VALUES"    { return token::VALUES;    }
"DELETE"    { return token::DELETE;    }
"FROM"      { return token::FROM;      }
"WHERE"     { return token::WHERE;     }
"UPDATE"    { return token::UPDATE;    }
"SET"       { return token::SET;       }
"SELECT"    { return token::SELECT;    }
"GROUP"     { return token::GROUP;     }
"BY"        { return token::BY;        }
"IS"        { return token::IS;        }
"IN"        { return token::IN;        }
"BETWEEN"   { return token::BETWEEN;   }
"LIKE"      { return token::LIKE;      }
"AND"       { return token::AND;       }
"OR"        { return token::OR;        }
"SUM"       { return token::SUM;       }
"AVG"       { return token::AVG;       }
"MAX"       { return token::MAX;       }
"MIN"       { return token::MIN;       }
"INT"       { return token::INT;       }
"SMALLINT"  { return token::SMALLINT;  }
"BIGINT"    { return token::BIGINT;    }
"FLOAT"     { return token::FLOAT;     }
"REAL"      { return token::FLOAT;     }
"DOUBLE"    { return token::DOUBLE;    }
"VARCHAR"   { return token::VARCHAR;   }
"STRING"    { return token::STRING;    }
"CHAR"      { return token::CHAR;      }
"BOOLEAN"   { return token::BOOLEAN;   }
"DATETIME"  { return token::DATETIME;  }
"DESC"      { return token::DESC;      }
"FOREIGN"   { return token::FOREIGN;   }
"REFERENCES"    { return token::REFERENCES;}


[0-9]+ {
    yylval->value = new Value(Value::VALUE_INT, yytext);
    return token::VALUE;
}

[0-9]+(\.)[0-9]+ {
    yylval->value = new Value(Value::VALUE_REAL, yytext);
    return token::VALUE;
}

'[^']*' {
    yylval->value = new Value(Value::VALUE_STRING, yytext);
    return token::VALUE;
}

[A-Za-z_][A-Za-z0-9_]* {
    strncpy(yylval->ident, yytext, 1024);
    return token::IDENT;
}

[ \t\n\x0B\f\r]+     { }
.       { driver.error(*yylloc, "unrecognized token"); }

%%
void Driver::scan_begin()
{
    if (!(yyin = fopen (file.c_str (), "r")))
        error(std::string("cannot open ") + file);
}

void Driver::scan_end()
{
    fclose(yyin);
}
