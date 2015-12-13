#ifndef PARSER_TREE_H_
#define PARSER_TREE_H_

#include "PrintWriter.h"

#include <string>
#include <vector>
#include <stdlib.h>
#include <assert.h>


class Visitor;

// AST
class Tree
{
public:
    virtual void printTo(PrintWriter& pw) = 0;
    virtual void accept(Visitor& v);
};

class Expr : public Tree
{
public:
    virtual void printTo(PrintWriter& pw) = 0;
    virtual void accept(Visitor& v);

    const static int OP_POS = 0;
    const static int OP_NEG = 1;
    const static int OP_ADD = 2;
    const static int OP_SUB = 3;
    const static int OP_MUL = 4;
    const static int OP_DIV = 5;
    const static int OP_MOD = 6;
};

class Value : public Expr
{
public:
    Value(int kind, const char *val) : kind(kind), val(std::string(val)) {}

    void printTo(PrintWriter& pw);
    virtual void accept(Visitor& v);

    int kind;
    std::string val;

    static const int VALUE_INT = 0;
    static const int VALUE_REAL = 1;
    static const int VALUE_STRING = 2;
};

class Variable : public Expr
{
public:
    Variable(const char *var) : var(std::string(var)) {}

    void printTo(PrintWriter& pw);
    virtual void accept(Visitor& v);

    std::string var;
};

class UnonExpr : public Expr
{
public:
    UnonExpr(Tree *expr, int op) : expr((Expr*)expr), op(op) {}

    void printTo(PrintWriter& pw);
    virtual void accept(Visitor& v);

    Expr *expr;
    int op;
};

class BinExpr : public Expr
{
public:
    BinExpr(Tree *left, Tree *right, int op) : left((Expr*)left), right((Expr*)right), op(op) {}

    void printTo(PrintWriter& pw);
    virtual void accept(Visitor& v);

    Expr *left;
    Expr *right;
    int op;
};

class TopLevel : public Tree
{
public:
    TopLevel(std::vector<Tree*> *stmts) : stmts(stmts) {}

    void printTo(PrintWriter& pw);
    virtual void accept(Visitor& v);

    std::vector<Tree*> *stmts;
};

class Stmt : public Tree
{
public:
    virtual void printTo(PrintWriter& pw) = 0;
    virtual void accept(Visitor& v);
};

class ListDB : public Stmt
{
public:
    ListDB() {}

    void printTo(PrintWriter& pw);
    virtual void accept(Visitor& v);
};

class CreateDBStmt : public Stmt
{
public:
    CreateDBStmt(const char *dbName) : db(std::string(dbName)) {}

    void printTo(PrintWriter& pw);
    virtual void accept(Visitor& v);

    std::string db;
};

class DropDBStmt : public Stmt
{
public:
    DropDBStmt(const char *dbName) : db(std::string(dbName)) {}

    void printTo(PrintWriter& pw);
    virtual void accept(Visitor& v);

    std::string db;
};

class UseDBStmt : public Stmt
{
public:
    UseDBStmt(const char *dbName) : db(std::string(dbName)) {}

    void printTo(PrintWriter& pw);
    virtual void accept(Visitor& v);

    std::string db;
};

class ListTB : public Stmt
{
public:
    ListTB() {}

    void printTo(PrintWriter& pw);
    virtual void accept(Visitor& v);
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
    virtual void accept(Visitor& v);

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
    virtual void accept(Visitor& v);

    Tree *type;
    std::string name;
    std::vector<int> *attrs;

    const static int ATTR_NOT_NULL = 0;
    const static int ATTR_UNIQUE = 1;
    const static int ATTR_AUTO_INCREMENT = 2;
};

class BoolExpr : public Tree
{
public:
    virtual void printTo(PrintWriter& pw) = 0;
    virtual void accept(Visitor& v);

    int op;

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

class NullExpr : public BoolExpr
{
public:
    NullExpr(const char *name, int op) : name(std::string(name)), op(op) {}

    void printTo(PrintWriter& pw);
    virtual void accept(Visitor& v);

    std::string name;
    int op;
};

class CompareExpr : public BoolExpr
{
public:
    CompareExpr(const char *left, Tree *right, int op)
        : left(std::string(left)), right((Value*)right), op(op) {}

    void printTo(PrintWriter& pw);
    virtual void accept(Visitor& v);

    std::string left;
    Value *right;
    int op;
};

class InExpr : public BoolExpr
{
public:
    InExpr(const char *left, std::vector<Tree*> *right, int op)
        : left(std::string(left)), right((std::vector<Value*>*)right), op(op) {}

    void printTo(PrintWriter& pw);
    virtual void accept(Visitor& v);

    std::string left;
    std::vector<Value*> *right;
    int op;
};

class BetweenExpr : public BoolExpr
{
public:
    BetweenExpr(const char *left, Value *rightL, Value *rightR, int op)
        : left(std::string(left)), rightL(rightL), rightR(rightR), op(op) {}

    void printTo(PrintWriter& pw);
    virtual void accept(Visitor& v);

    std::string left;
    Value *rightL;
    Value *rightR;
    int op;
};

class ComplexExpr : public BoolExpr
{
public:
    ComplexExpr(Tree *left, Tree *right, int op)
        : left((BoolExpr*)left), right((BoolExpr*)right), op(op) {}

    void printTo(PrintWriter& pw);
    virtual void accept(Visitor& v);

    int op;
    BoolExpr *left;
    BoolExpr *right;
};

class Check : public Tree
{
public:
    Check() : empty(true) {}
    Check(Tree *check) : check((BoolExpr*)check), empty(false) {}

    void printTo(PrintWriter& pw);
    virtual void accept(Visitor& v);

    BoolExpr *check;
    bool empty;
};

class PrimaryKey : public Tree
{
public:
    PrimaryKey() : empty(true) {}
    PrimaryKey(const char *key) : key(std::string(key)), empty(false) {}

    void printTo(PrintWriter& pw);
    virtual void accept(Visitor& v);

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
    virtual void accept(Visitor& v);

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
    virtual void accept(Visitor& v);

    std::string tb;
};

class ShowTBStmt : public Stmt
{
public:
    ShowTBStmt(const char *tbName) : tb(std::string(tbName)) {}

    void printTo(PrintWriter& pw);
    virtual void accept(Visitor& v);

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
    virtual void accept(Visitor& v);

    std::vector<std::string> *cols;
};

class InsertStmt : public Stmt
{
public:
    InsertStmt(const char *tbName, Tree *cols, std::vector<Tree*> *values)
        : tbName(std::string(tbName)), cols((Columns*)cols), values((std::vector<Value*>*)values) {}

    void printTo(PrintWriter& pw);
    virtual void accept(Visitor& v);

    std::string tbName;
    Columns *cols;
    std::vector<Value*> *values;
};

class DeleteStmt : public Stmt
{
public:
    DeleteStmt(const char *tbName, Tree *where)
        : tbName(std::string(tbName)), where((BoolExpr*)where) {}

    void printTo(PrintWriter& pw);
    virtual void accept(Visitor& v);

    std::string tbName;
    BoolExpr *where;
};

class Eq : public Tree
{
public:
    Eq(const char *colName, Tree *expr) : colName(std::string(colName)), expr((Expr*)expr) {}

    void printTo(PrintWriter& pw);
    virtual void accept(Visitor& v);

    std::string colName;
    Expr *expr;
};

class UpdateStmt : public Stmt
{
public:
    UpdateStmt(const char *tbName, std::vector<Tree*> *assigns, Tree *where)
        : tbName(std::string(tbName)), where((BoolExpr*)where)
    {
        eqs = new std::vector<Eq*>;
        for (auto& eq : *assigns) {
            eqs->push_back((Eq*)eq);
        }
    }

    void printTo(PrintWriter& pw);
    virtual void accept(Visitor& v);

    std::string tbName;
    std::vector<Eq*> *eqs;
    BoolExpr *where;
};

class Selector : public Tree
{
public:
    Selector(const char *tbName, const char *colName)
        : func(FUNC_NULL), tbName(std::string(tbName)), colName(std::string(colName)) {}
    Selector(int func, const char *colName)
        : func(func), colName(std::string(colName)) {}

    void printTo(PrintWriter& pw);
    virtual void accept(Visitor& v);

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
    virtual void accept(Visitor& v);

    std::vector<Selector*> *selectors;
    bool all;
};

class Where : public Tree
{
public:
    Where() : empty(true) {}
    Where(Tree *where) : where((BoolExpr*)where), empty(false) {}

    void printTo(PrintWriter& pw) {}
    virtual void accept(Visitor& v);

    BoolExpr *where;
    bool empty;
};

class GroupBy : public Tree
{
public:
    GroupBy() : empty(true) {}
    GroupBy(const char *colName) : colName(std::string(colName)), empty(false) {}

    void printTo(PrintWriter& pw) {}
    virtual void accept(Visitor& v);

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
    virtual void accept(Visitor& v);

    std::string tbName;
    Selectors *sel;
    Where *where;
    GroupBy *gb;
};


// Visitor
class Visitor
{
public:
    virtual void visitTree(Tree *that) { assert(false); }
    virtual void visitExpr(Expr *that) { visitTree(that); }
    virtual void visitValue(Value *that) { visitTree(that); }
    virtual void visitVariable(Variable *that) { visitTree(that); }
    virtual void visitUnonExpr(UnonExpr *that) { visitTree(that); }
    virtual void visitBinExpr(BinExpr *that) { visitTree(that); }
    virtual void visitTopLevel(TopLevel *that) { visitTree(that); }
    virtual void visitStmt(Stmt *that) { visitTree(that); }
    virtual void visitListDB(ListDB *that) { visitTree(that); }
    virtual void visitCreateDBStmt(CreateDBStmt *that) { visitTree(that); }
    virtual void visitDropDBStmt(DropDBStmt *that) { visitTree(that); }
    virtual void visitUseDBStmt(UseDBStmt *that) { visitTree(that); }
    virtual void visitListTB(ListTB *that) { visitTree(that); }
    virtual void visitType(Type *that) { visitTree(that); }
    virtual void visitField(Field *that) { visitTree(that); }
    virtual void visitBoolExpr(BoolExpr *that) { visitTree(that); }
    virtual void visitNullExpr(NullExpr *that) { visitTree(that); }
    virtual void visitCompareExpr(CompareExpr *that) { visitTree(that); }
    virtual void visitInExpr(InExpr *that) { visitTree(that); }
    virtual void visitBetweenExpr(BetweenExpr *that) { visitTree(that); }
    virtual void visitComplexExpr(ComplexExpr *that) { visitTree(that); }
    virtual void visitCheck(Check *that) { visitTree(that); }
    virtual void visitPrimaryKey(PrimaryKey *that) { visitTree(that); }
    virtual void visitCreateTBStmt(CreateTBStmt *that) { visitTree(that); }
    virtual void visitDropTBStmt(DropTBStmt *that) { visitTree(that); }
    virtual void visitShowTBStmt(ShowTBStmt *that) { visitTree(that); }
    virtual void visitColumns(Columns *that) { visitTree(that); }
    virtual void visitInsertStmt(InsertStmt *that) { visitTree(that); }
    virtual void visitDeleteStmt(DeleteStmt *that) { visitTree(that); }
    virtual void visitEq(Eq *that) { visitTree(that); }
    virtual void visitUpdateStmt(UpdateStmt *that) { visitTree(that); }
    virtual void visitSelector(Selector *that) { visitTree(that); }
    virtual void visitSelectors(Selectors *that) { visitTree(that); }
    virtual void visitWhere(Where *that) { visitTree(that); }
    virtual void visitGroupBy(GroupBy *that) { visitTree(that); }
    virtual void visitSelectStmt(SelectStmt *that) { visitTree(that); }
};

#endif // PARSER_TREE_H_
