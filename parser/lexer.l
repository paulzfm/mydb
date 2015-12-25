%{
#include "SemValue.h"
#include "parser.tab.hpp"
%}

%option noyywrap
%option caseless

%% /* tokens */

"("      { return LP; }
")"      { return RP; }
","      { return COMMA; }
";"      { return STOP; }

"="      { return EQ; }
"<>"     { return NE; }
"<="     { return LE; }
">="     { return GE; }
"<"      { return LT; }
">"      { return GT; }

"+"      { return PLUS; }
"-"      { return MINUS; }
"*"      { return MUL; }
"/"      { return DIV; }
"%"      { return MOD; }
"."      { return DOT; }

"DATABASE"  { return DATABASE;  }
"DATABASES" { return DATABASES; }
"TABLE"     { return TABLE;     }
"TABLES"    { return TABLES;    }
"SHOW"      { return SHOW;      }
"CREATE"    { return CREATE;    }
"DROP"      { return DROP;      }
"USE"       { return USE;       }
"CHECK"     { return CHECK;     }
"PRIMARY"   { return PRIMARY;   }
"KEY"       { return KEY;       }
"UNIQUE"    { return UNIQUE;    }
"NOT"       { return NOT;       }
"NULL"      { return NIL;      }
"AUTO_INCREMENT"    { return AUTO_INCREMENT; }
"INSERT"    { return INSERT;    }
"INTO"      { return INTO;      }
"VALUES"    { return VALUES;    }
"DELETE"    { return DELETE;    }
"FROM"      { return FROM;      }
"WHERE"     { return WHERE;     }
"UPDATE"    { return UPDATE;    }
"SET"       { return SET;       }
"SELECT"    { return SELECT;    }
"GROUP"     { return GROUP;     }
"BY"        { return BY;        }
"IS"        { return IS;        }
"IN"        { return IN;        }
"BETWEEN"   { return BETWEEN;   }
"LIKE"      { return LIKE;      }
"AND"       { return AND;       }
"OR"        { return OR;        }
"SUM"       { return SUM;       }
"AVG"       { return AVG;       }
"MAX"       { return MAX;       }
"MIN"       { return MIN;       }
"INT"       { return INT;       }
"SMALLINT"  { return SMALLINT;  }
"BIGINT"    { return BIGINT;    }
"FLOAT"     { return FLOAT;     }
"REAL"      { return FLOAT;     }
"DOUBLE"    { return DOUBLE;    }
"VARCHAR"   { return VARCHAR;   }
"STRING"    { return STRING;    }
"CHAR"      { return CHAR;      }
"BOOLEAN"   { return BOOLEAN;   }
"DATETIME"  { return DATETIME;  }
"DESC"      { return DESC;      }
"FOREIGN"   { return FOREIGN;   }
"REFERENCES"    { return REFERENCES;}


[0-9]+ {
    yylval.value = new Value(Value::VALUE_INT, yytext);
    return VALUE;
}

[0-9]+(\.)[0-9]+ {
    yylval.value = new Value(Value::VALUE_REAL, yytext);
    return VALUE;
}

'[^']*' {
    yylval.value = new Value(Value::VALUE_STRING, yytext);
    return VALUE;
}

[A-Za-z_][A-Za-z0-9_]* {
    strncpy(yylval.ident, yytext, 1024);
    return IDENT;
}

[ \t\n]+ { }
<<EOF>>  { return 0; }
.        { printf("Unrecognized token!"); exit(1); }

%%
