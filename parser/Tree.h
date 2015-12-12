#ifndef PARSER_TREE_H_
#define PARSER_TREE_H_

#include "PrintWriter.h"

#include <string>
#include <vector>
#include <stdlib.h>


class Tree
{
public:
    virtual void printTo(PrintWriter& pw) = 0;
};

class Value : public Tree
{
public:
    Value(int kind, const char *val) : kind(kind), val(std::string(val)) {}

    void printTo(PrintWriter& pw);

    int kind;
    std::string val;

    static const int VALUE_INT = 0;
    static const int VALUE_REAL = 1;
    static const int VALUE_STRING = 2;
};

class TopLevel : public Tree
{
public:
    TopLevel(std::vector<Tree*> *stmts) : stmts(stmts) {}
    void printTo(PrintWriter& pw);

    std::vector<Tree*> *stmts;
};

class Stmt : public Tree
{
public:
    Stmt() {}
    virtual void printTo(PrintWriter& pw) = 0;
};

class ListDB : public Stmt
{
public:
    ListDB() {}
    void printTo(PrintWriter& pw);
};

class CreateDBStmt : public Stmt
{
public:
    CreateDBStmt(const char *dbName) : db(std::string(dbName)) {}
    void printTo(PrintWriter& pw);

    std::string db;
};

class DropDBStmt : public Stmt
{
public:
    DropDBStmt(const char *dbName) : db(std::string(dbName)) {}
    void printTo(PrintWriter& pw);

    std::string db;
};

class UseDBStmt : public Stmt
{
public:
    UseDBStmt(const char *dbName) : db(std::string(dbName)) {}
    void printTo(PrintWriter& pw);

    std::string db;
};

class ListTB : public Stmt
{
public:
    ListTB() {}
    void printTo(PrintWriter& pw);
};

class Type : public Tree
{
public:
    Type(int kind, Value *len = nullptr) : kind(kind)
    {
        length = 255;
        if (len && len->kind == Value::VALUE_INT) {
            length = atoi(len->val.c_str());
        }
    }

    void printTo(PrintWriter& pw);

    int kind;
    int length;

    const static int TYPE_INT = 0;
    const static int TYPE_SMALL_INT = 1;
    const static int TYPE_BIG_INT = 2;
    const static int TYPE_FLOAT = 3;
    const static int TYPE_DOUBLE = 4;
    const static int TYPE_STRING = 5;
    const static int TYPE_CHAR = 6;
    const static int TYPE_BOOLEAN = 7;
    const static int TYPE_DATETIME = 8;
};

class Field : public Tree
{
public:
    Field(Tree *type, const char *name, std::vector<int> *attrs)
        : type(type), name(std::string(name)), attrs(attrs) {}
    void printTo(PrintWriter& pw);

    Tree *type;
    std::string name;
    std::vector<int> *attrs;

    const static int ATTR_NOT_NULL = 0;
    const static int ATTR_UNIQUE = 1;
    const static int ATTR_AUTO_INCREMENT = 2;
};

class Expr : public Tree
{
public:
    virtual void printTo(PrintWriter& pw) = 0;

    const static int OP_EQ = 0;
    const static int OP_NE = 1;
    const static int OP_GE = 2;
    const static int OP_LE = 3;
    const static int OP_GT = 4;
    const static int OP_LT = 5;
    const static int OP_IN = 6;
    const static int OP_NOT_IN = 7;
    const static int OP_LIKE = 8;
    const static int OP_NOT_LIKE = 9;
    const static int OP_BETWEEN = 10;
    const static int OP_NOT_BETWEEN = 11;
    const static int OP_AND = 12;
    const static int OP_OR = 13;
    const static int OP_IS_NULL = 14;
    const static int OP_IS_NOT_NULL = 15;
};

class UnonExpr : public Expr
{
public:
    UnonExpr(const char *name, int op) : name(std::string(name)), op(op) {}
    void printTo(PrintWriter& pw);

    std::string name;
    int op;
};

class BinExpr : public Expr
{
public:
    BinExpr(const char *left, Tree *right, int op)
        : left(std::string(left)), right((Value*)right), op(op) {}
    void printTo(PrintWriter& pw);

    std::string left;
    Value *right;
    int op;
};

class InExpr : public Expr
{
public:
    InExpr(const char *left, std::vector<Tree*> *right, int op)
        : left(std::string(left)), right((std::vector<Value*>*)right), op(op) {}
    void printTo(PrintWriter& pw);

    std::string left;
    std::vector<Value*> *right;
    int op;
};

class BetweenExpr : public Expr
{
public:
    BetweenExpr(const char *left, Value *rightL, Value *rightR, int op)
        : left(std::string(left)), rightL(rightL), rightR(rightR), op(op) {}
    void printTo(PrintWriter& pw);

    std::string left;
    Value *rightL;
    Value *rightR;
    int op;
};

class ComExpr : public Expr
{
public:
    ComExpr(Tree *left, Tree *right, int op) : left((Expr*)left), right((Expr*)right), op(op) {}
    void printTo(PrintWriter& pw);

    int op;
    Expr *left;
    Expr *right;
};

class Check : public Tree
{
public:
    Check() : empty(true) {}
    Check(Tree *check) : check((Expr*)check), empty(false) {}
    void printTo(PrintWriter& pw);

    Expr *check;
    bool empty;
};

class PrimaryKey : public Tree
{
public:
    PrimaryKey() : empty(true) {}
    PrimaryKey(const char *key) : key(std::string(key)), empty(false) {}
    void printTo(PrintWriter& pw);

    std::string key;
    bool empty;
};

class CreateTBStmt : public Stmt
{
public:
    CreateTBStmt(const char *tbName, std::vector<Tree*> *fields, Tree *check, Tree *pkey)
        : tb(std::string(tbName)), fields((std::vector<Field*>*)fields),
          check((Check*)check), pkey((PrimaryKey*)pkey) {}
    void printTo(PrintWriter& pw);

    std::string tb;
    std::vector<Field*> *fields;
    Check *check;
    PrimaryKey *pkey;
};

class DropTBStmt : public Stmt
{
public:
    DropTBStmt(const char *tbName) : tb(std::string(tbName)) {}
    void printTo(PrintWriter& pw);

    std::string tb;
};

class ShowTBStmt : public Stmt
{
public:
    ShowTBStmt(const char *tbName) : tb(std::string(tbName)) {}
    void printTo(PrintWriter& pw);

    std::string tb;
};

class Columns : public Tree
{
public:
    Columns() { cols = new std::vector<std::string>; }
    Columns(std::vector<const char*> *idents)
    {
        cols = new std::vector<std::string>;
        for (auto& ident : *idents) {
            cols->push_back(std::string(ident));
        }
    }

    void printTo(PrintWriter& pw);

    std::vector<std::string> *cols;
};

class InsertStmt : public Stmt
{
public:
    InsertStmt(const char *tbName, Tree *cols, std::vector<Tree*> *values)
        : tbName(std::string(tbName)), cols((Columns*)cols), values((std::vector<Value*>*)values) {}
    void printTo(PrintWriter& pw);

    std::string tbName;
    Columns *cols;
    std::vector<Value*> *values;
};

class DeleteStmt : public Stmt
{
public:
    DeleteStmt(const char *tbName, Tree *where)
        : tbName(std::string(tbName)), where((Expr*)where) {}
    void printTo(PrintWriter& pw);

    std::string tbName;
    Expr *where;
};

class Eq : public Tree
{
public:
    Eq(const char *colName, Value *value) : colName(std::string(colName)), value(value) {}
    void printTo(PrintWriter& pw);

    std::string colName;
    Value *value;
};

class UpdateStmt : public Stmt
{
public:
    UpdateStmt(const char *tbName, std::vector<Tree*> *assigns, Tree *where)
        : tbName(std::string(tbName)), where((Expr*)where)
    {
        eqs = new std::vector<Eq*>;
        for (auto& eq : *assigns) {
            eqs->push_back((Eq*)eq);
        }
    }

    void printTo(PrintWriter& pw);

    std::string tbName;
    std::vector<Eq*> *eqs;
    Expr *where;
};

class Selector : public Tree
{
public:
    Selector(const char *tbName, const char *colName)
        : func(FUNC_NULL), tbName(std::string(tbName)), colName(std::string(colName)) {}
    Selector(int func, const char *colName)
        : func(func), colName(std::string(colName)) {}

    void printTo(PrintWriter& pw);

    std::string tbName;
    std::string colName;
    int func;

    const static int FUNC_NULL = 0;
    const static int FUNC_SUM = 1;
    const static int FUNC_AVG = 2;
    const static int FUNC_MAX = 3;
    const static int FUNC_MIN = 4;
};

class Selectors : public Tree
{
public:
    Selectors() : all(true) {}
    Selectors(std::vector<Tree*> *sels) : all(false)
    {
        selectors = new std::vector<Selector*>;
        for (auto& sel : *sels) {
            selectors->push_back((Selector*)sel);
        }
    }

    void printTo(PrintWriter& pw);

    std::vector<Selector*> *selectors;
    bool all;
};

class Where : public Tree
{
public:
    Where() : empty(true) {}
    Where(Tree *where) : where((Expr*)where), empty(false) {}

    void printTo(PrintWriter& pw) {}

    Expr *where;
    bool empty;
};

class GroupBy : public Tree
{
public:
    GroupBy() : empty(true) {}
    GroupBy(const char *colName) : colName(std::string(colName)), empty(false) {}

    void printTo(PrintWriter& pw) {}

    std::string colName;
    bool empty;
};

class SelectStmt : public Stmt
{
public:
    SelectStmt(const char *tbName, Tree *sel, Tree *where, Tree *gb)
        : tbName(std::string(tbName)), sel((Selectors*)sel), where((Where*)where),
          gb((GroupBy*)gb) {}
    void printTo(PrintWriter& pw);

    std::string tbName;
    Selectors *sel;
    Where *where;
    GroupBy *gb;
};

#endif // PARSER_TREE_H_
