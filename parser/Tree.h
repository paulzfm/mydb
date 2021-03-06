#ifndef PARSER_TREE_H_
#define PARSER_TREE_H_

#include "PrintWriter.h"
#include "../record/types.h"
#include "../lib/rapidjson/document.h"

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
    virtual bool accept(Visitor *v) = 0;
};

class Expr : public Tree
{
public:
    Expr(int op) : op(op) {}

    virtual void printTo(PrintWriter& pw) = 0;
    virtual bool accept(Visitor *v) = 0;
    virtual void fillTable(std::string &tbName) = 0;
    virtual rapidjson::Value toJSONValue(rapidjson::Document::AllocatorType& allocator) = 0;

    int op;

    const static int OP_POS = 0;
    const static int OP_NEG = 1;
    const static int OP_ADD = 2;
    const static int OP_SUB = 3;
    const static int OP_MUL = 4;
    const static int OP_DIV = 5;
    const static int OP_MOD = 6;
    const static int OP_COL = 7;
    const static int OP_VAL = 8;
};

class Col : public Expr
{
public:
    Col(const char *colName)
        : Expr(OP_COL), tb(""), col(std::string(colName)) {}
    Col(const char *tbName, const char *colName)
        : Expr(OP_COL), tb(std::string(tbName)), col(std::string(colName)) {}

    void printTo(PrintWriter& pw);
    virtual bool accept(Visitor *v);
    void fillTable(std::string &tbName);
    virtual rapidjson::Value toJSONValue(rapidjson::Document::AllocatorType& allocator);

    std::string tb;
    std::string col;
};

class Type : public Tree
{
public:
    Type(int kind, int len = 10) : kind(kind), length(len) {}

    void printTo(PrintWriter& pw);
    virtual bool accept(Visitor *v);
    char toDType();
    int getSize();

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
    // TODO: remove DATETIME
    const static int TYPE_DATETIME = 8;
};

class Value : public Expr
{
public:
    Value(int kind) : Expr(OP_VAL), kind(kind) {}
    Value(int kind, const char *val) : Expr(OP_VAL), kind(kind), val(std::string(val)) {}

    void printTo(PrintWriter& pw);
    virtual bool accept(Visitor *v);
    bool compatibleWith(Type *type);
    virtual void fillTable(std::string &tbName) {}
    virtual rapidjson::Value toJSONValue(rapidjson::Document::AllocatorType& allocator);

    int kind;
    std::string val;

    static const int VALUE_INT = 0;
    static const int VALUE_REAL = 1;
    static const int VALUE_STRING = 2;
    static const int VALUE_NULL = 3;
    static const int VALUE_TRUE = 4;
    static const int VALUE_FALSE = 5;
};

class CValue : public Tree
{
public:
    CValue(Value *v, bool)
    {
        if (v->kind == Value::VALUE_INT || v->kind == Value::VALUE_REAL) { // neg
            char buf[2048];
            *buf = '-';
            strcat(buf + 1, v->val.c_str());
            value = new Value(v->kind, buf);
        } else {
            value = v;
        }
    }

    CValue(Value *v)
    {
        value = v;
    }

    CValue(Tree *v)
    {
        value = (Value*)v;
    }

    void printTo(PrintWriter& pw) {}
    virtual bool accept(Visitor *v);

    Value *value;
};

class UnonExpr : public Expr
{
public:
    UnonExpr(Tree *expr, int op) : Expr(op), expr((Expr*)expr) {}

    void printTo(PrintWriter& pw);
    virtual bool accept(Visitor *v);
    virtual void fillTable(std::string &tbName);
    virtual rapidjson::Value toJSONValue(rapidjson::Document::AllocatorType& allocator);

    Expr *expr;
};

class BinExpr : public Expr
{
public:
    BinExpr(Tree *left, Tree *right, int op) : Expr(op), left((Expr*)left), right((Expr*)right) {}

    void printTo(PrintWriter& pw);
    virtual bool accept(Visitor *v);
    virtual void fillTable(std::string &tbName);
    virtual rapidjson::Value toJSONValue(rapidjson::Document::AllocatorType& allocator);

    Expr *left;
    Expr *right;
};

class TopLevel : public Tree
{
public:
    TopLevel(std::vector<Tree*> *stmts) : stmts(stmts) {}

    void printTo(PrintWriter& pw);
    virtual bool accept(Visitor *v);

    std::vector<Tree*> *stmts;
};

class Stmt : public Tree
{
public:
    virtual void printTo(PrintWriter& pw) = 0;
    virtual bool accept(Visitor *v) = 0;
};

class ListDB : public Stmt
{
public:
    ListDB() {}

    void printTo(PrintWriter& pw);
    virtual bool accept(Visitor *v);
};

class CreateDBStmt : public Stmt
{
public:
    CreateDBStmt(const char *dbName) : db(std::string(dbName)) {}

    void printTo(PrintWriter& pw);
    virtual bool accept(Visitor *v);

    std::string db;
};

class DropDBStmt : public Stmt
{
public:
    DropDBStmt(const char *dbName) : db(std::string(dbName)) {}

    void printTo(PrintWriter& pw);
    virtual bool accept(Visitor *v);

    std::string db;
};

class UseDBStmt : public Stmt
{
public:
    UseDBStmt(const char *dbName) : db(std::string(dbName)) {}

    void printTo(PrintWriter& pw);
    virtual bool accept(Visitor *v);

    std::string db;
};

class ListTB : public Stmt
{
public:
    ListTB() {}

    void printTo(PrintWriter& pw);
    virtual bool accept(Visitor *v);
};

class BoolExpr : public Tree
{
public:
    BoolExpr(int op) : op(op) {}

    virtual void printTo(PrintWriter& pw) = 0;
    virtual bool accept(Visitor *v) = 0;
    virtual void fillTable(std::string &tbName) = 0;
    virtual rapidjson::Value toJSONValue(rapidjson::Document::AllocatorType& allocator) = 0;

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
    const static int OP_NONE = 16;
};

class BoolValue : public BoolExpr
{
public:
    BoolValue(bool val = true) : BoolExpr(OP_NONE), val(val) {}

    void printTo(PrintWriter& pw);
    virtual bool accept(Visitor *v);
    virtual void fillTable(std::string &tbName) {}
    virtual rapidjson::Value toJSONValue(rapidjson::Document::AllocatorType& allocator);

    bool val;
};

class NullExpr : public BoolExpr
{
public:
    NullExpr(Tree *col, int op) : BoolExpr(op), name((Col*)col) {}

    void printTo(PrintWriter& pw);
    virtual bool accept(Visitor *v);
    virtual void fillTable(std::string &tbName);
    virtual rapidjson::Value toJSONValue(rapidjson::Document::AllocatorType& allocator);

    Col *name;
};

class CompareExpr : public BoolExpr
{
public:
    CompareExpr(Tree *left, Tree *right, int op)
        : BoolExpr(op), left((Col*)left), right((Expr*)right) {}

    void printTo(PrintWriter& pw);
    virtual bool accept(Visitor *v);
    virtual void fillTable(std::string &tbName);
    virtual rapidjson::Value toJSONValue(rapidjson::Document::AllocatorType& allocator);

    Col *left;
    Expr *right;
};

class InExpr : public BoolExpr
{
public:
    InExpr(Tree *left, std::vector<Tree*> *set, int op)
        : BoolExpr(op), left((Col*)left)
    {
        right = new std::vector<Value*>;
        for (auto& val : *set) {
            right->push_back(((CValue*)val)->value);
        }
    }

    void printTo(PrintWriter& pw);
    virtual bool accept(Visitor *v);
    virtual void fillTable(std::string &tbName);
    virtual rapidjson::Value toJSONValue(rapidjson::Document::AllocatorType& allocator);

    Col *left;
    std::vector<Value*> *right;
};

class BetweenExpr : public BoolExpr
{
public:
    BetweenExpr(Tree *left, CValue *rightL, CValue *rightR, int op)
        : BoolExpr(op), left((Col*)left), rightL(rightL->value), rightR(rightR->value) {}
    BetweenExpr(Tree *left, Tree *rightL, Tree *rightR, int op)
        : BoolExpr(op), left((Col*)left), rightL((Value*)rightL), rightR((Value*)rightR) {}

    void printTo(PrintWriter& pw);
    virtual bool accept(Visitor *v);
    virtual void fillTable(std::string &tbName);
    virtual rapidjson::Value toJSONValue(rapidjson::Document::AllocatorType& allocator);

    Col *left;
    Value *rightL;
    Value *rightR;
};

class ComplexExpr : public BoolExpr
{
public:
    ComplexExpr(Tree *left, Tree *right, int op)
        : BoolExpr(op), left((BoolExpr*)left), right((BoolExpr*)right) {}

    void printTo(PrintWriter& pw);
    virtual bool accept(Visitor *v);
    virtual void fillTable(std::string &tbName);
    virtual rapidjson::Value toJSONValue(rapidjson::Document::AllocatorType& allocator);

    BoolExpr *left;
    BoolExpr *right;
};

class ColConstraint : public Tree
{
public:
    ColConstraint(int kind) : kind(kind) {}
    virtual void printTo(PrintWriter& pw) = 0;
    virtual bool accept(Visitor *v) = 0;

    const static int CONS_CHECK = 0;
    const static int CONS_PKEY = 1;
    const static int CONS_FKEY = 2;
    const static int CONS_FIELD = 3;

    int kind;
};

class Attr : public Tree
{
public:
    Attr(int kind) : kind(kind) {}
    Attr(Tree *val) : def((Value*)val), kind(ATTR_DEFAULT) {}

    virtual void printTo(PrintWriter& pw) {}
    virtual bool accept(Visitor *v) { return false; }

    const static int ATTR_NOT_NULL = 0;
    const static int ATTR_UNIQUE = 1;
    const static int ATTR_AUTO_INCREMENT = 6;
    const static int ATTR_DEFAULT = 5;

    int kind;
    Value *def;
};

class Field : public ColConstraint
{
public:
    Field(Tree *type, const char *name, std::vector<Tree*> *attributes)
        : ColConstraint(ColConstraint::CONS_FIELD), type((Type*)type), name(std::string(name))
    {
        attrs = new std::vector<Attr*>;
        for (auto &attr : *attributes) {
            attrs->push_back((Attr*)attr);
        }
    }

    void printTo(PrintWriter& pw);
    virtual bool accept(Visitor *v);

    Type *type;
    std::string name;
    std::vector<Attr*> *attrs;
};

class Check : public ColConstraint
{
public:
    Check(Tree *check)
        : ColConstraint(ColConstraint::CONS_CHECK), check((BoolExpr*)check) {}

    void printTo(PrintWriter& pw);
    virtual bool accept(Visitor *v) { return false; }

    BoolExpr *check;
};

class PrimaryKey : public ColConstraint
{
public:
    PrimaryKey(const char *key)
        : ColConstraint(ColConstraint::CONS_PKEY), key(std::string(key)) {}

    void printTo(PrintWriter& pw);
    virtual bool accept(Visitor *v) { return false; }

    std::string key;
};

class ForeignKey : public ColConstraint
{
public:
    ForeignKey(const char *key, const char *rtb, const char *rkey)
        : ColConstraint(ColConstraint::CONS_FKEY), key(std::string(key)),
          rtb(std::string(rtb)), rkey(std::string(rkey)) {}

    void printTo(PrintWriter& pw);
    virtual bool accept(Visitor *v) { return false; }

    std::string key;
    std::string rtb;
    std::string rkey;
};

class CreateTBStmt : public Stmt
{
public:
    CreateTBStmt(const char *tbName, std::vector<Tree*> *constrains)
        : tb(std::string(tbName))
    {
        fields = new std::vector<Field*>;
        checks = new std::vector<Check*>;
        pkeys = new std::vector<PrimaryKey*>;
        fkeys = new std::vector<ForeignKey*>;

        for (auto& con : *constrains) {
            ColConstraint *c = (ColConstraint*)con;
            switch (c->kind) {
                case ColConstraint::CONS_FIELD:
                    fields->push_back((Field*)c);
                    break;
                case ColConstraint::CONS_CHECK:
                    checks->push_back((Check*)c);
                    break;
                case ColConstraint::CONS_PKEY:
                    pkeys->push_back((PrimaryKey*)c);
                    break;
                case ColConstraint::CONS_FKEY:
                    fkeys->push_back((ForeignKey*)c);
                    break;
            }
        }
    }

    void printTo(PrintWriter& pw);
    virtual bool accept(Visitor *v);
    bool exists(std::string& col);
    int indexOf(std::string& col);

    std::string tb;
    std::vector<Field*> *fields;
    std::vector<Check*> *checks;
    std::vector<PrimaryKey*> *pkeys;
    std::vector<ForeignKey*> *fkeys;
};

class DropTBStmt : public Stmt
{
public:
    DropTBStmt(const char *tbName) : tb(std::string(tbName)) {}

    void printTo(PrintWriter& pw);
    virtual bool accept(Visitor *v);

    std::string tb;
};

class ShowTBStmt : public Stmt
{
public:
    ShowTBStmt(const char *tbName) : tb(std::string(tbName)) {}

    void printTo(PrintWriter& pw);
    virtual bool accept(Visitor *v);

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
            delete[] ident;
        }
    }

    void printTo(PrintWriter& pw);
    virtual bool accept(Visitor *v);

    std::vector<std::string> *cols;
};

class InsertStmt : public Stmt
{
public:
    InsertStmt(const char *tbName, Tree *cols, std::vector< std::vector<Tree*>* > *sets)
        : tb(std::string(tbName)), cols((Columns*)cols)
    {
        values = new std::vector< std::vector<Value*>* >;
        for (auto& set : *sets) {
            std::vector<Value*> *tmp = new std::vector<Value*>;
            for (auto& cvalue : *(std::vector<CValue*>*)set) {
                tmp->push_back(cvalue->value);
            }
            values->push_back(tmp);
        }
    }

    void printTo(PrintWriter& pw);
    virtual bool accept(Visitor *v);

    std::string tb;
    Columns *cols;
    std::vector< std::vector<Value*>* > *values;
};

class DeleteStmt : public Stmt
{
public:
    DeleteStmt(const char *tbName, Tree *where)
        : tb(std::string(tbName)), where((BoolExpr*)where) {}

    void printTo(PrintWriter& pw);
    virtual bool accept(Visitor *v);

    std::string tb;
    BoolExpr *where;
};

class Eq : public Tree
{
public:
    Eq(const char *colName, Tree *expr) : colName(std::string(colName)), expr((Expr*)expr) {}

    void printTo(PrintWriter& pw);
    virtual bool accept(Visitor *v);

    std::string colName;
    Expr *expr;
};

class UpdateStmt : public Stmt
{
public:
    UpdateStmt(const char *tbName, std::vector<Tree*> *assigns, Tree *where)
        : tb(std::string(tbName)), where((BoolExpr*)where)
    {
        eqs = new std::vector<Eq*>;
        for (auto& eq : *assigns) {
            eqs->push_back((Eq*)eq);
        }
    }

    void printTo(PrintWriter& pw);
    virtual bool accept(Visitor *v);

    std::string tb;
    std::vector<Eq*> *eqs;
    BoolExpr *where;
};

class Selector : public Tree
{
public:
    Selector(Tree *col)
        : func(FUNC_NULL), col((Col*)col) {}
    Selector(int func, Tree *col)
        : func(func), col((Col*)col) {}

    void printTo(PrintWriter& pw);
    virtual bool accept(Visitor *v);

    Col *col;
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
    virtual bool accept(Visitor *v);

    std::vector<Selector*> *selectors;
    bool all;
};

class Where : public Tree
{
public:
    Where() : empty(true) {}
    Where(Tree *where) : where((BoolExpr*)where), empty(false) {}

    void printTo(PrintWriter& pw) {}
    virtual bool accept(Visitor *v) { return false; }

    BoolExpr *where;
    bool empty;
};

class GroupBy : public Tree
{
public:
    GroupBy() : empty(true) {}
    GroupBy(const char *colName) : col(std::string(colName)), empty(false) {}
    GroupBy(const char *tbName, const char *colName)
        : tb(std::string(tbName)), col(std::string(colName)), empty(false) {}

    void printTo(PrintWriter& pw) {}
    virtual bool accept(Visitor *v) { return false; }
    void fillTable(std::string &tbName);

    std::string tb;
    std::string col;
    bool empty;
};

class SelectStmt : public Stmt
{
public:
    SelectStmt(std::vector<const char*> *tbNames, Tree *selectors, Tree *whereClause, Tree *group)
        : sel((Selectors*)selectors), where((Where*)whereClause), gb((GroupBy*)group)
    {
        for (auto& tb : *tbNames) {
            tbs.push_back(std::string(tb));
        }

        if (tbs.size() == 1) {
            if (!sel->all) {
                for (auto& s : *sel->selectors) {
                    s->col->fillTable(tbs[0]);
                }
            }

            if (!where->empty) {
                where->where->fillTable(tbs[0]);
            }

            if (!gb->empty) {
                gb->fillTable(tbs[0]);
            }
        }
    }

    void printTo(PrintWriter& pw);
    virtual bool accept(Visitor *v);

    std::vector<std::string> tbs;
    Selectors *sel;
    Where *where;
    GroupBy *gb;
};

class CreateIdxStmt : public Stmt
{
public:
    CreateIdxStmt(const char *tbName, const char *colName)
        : tb(std::string(tbName)), col(std::string(colName)) {}

    void printTo(PrintWriter& pw);
    virtual bool accept(Visitor *v);

    std::string tb;
    std::string col;
};

class DropIdxStmt : public Stmt
{
public:
    DropIdxStmt(const char *tbName, const char *colName)
        : tb(std::string(tbName)), col(std::string(colName)) {}

    void printTo(PrintWriter& pw);
    virtual bool accept(Visitor *v);

    std::string tb;
    std::string col;
};

Tree* toTreeNode(const rapidjson::Value& val);


// Visitor
class Visitor
{
public:
    virtual bool visitTree(Tree *that) { assert(false); return false; }
    virtual bool visitValue(Value *that) { return visitTree(that); }
    virtual bool visitCol(Col *that) { return visitTree(that); }
    virtual bool visitUnonExpr(UnonExpr *that) { return visitTree(that); }
    virtual bool visitBinExpr(BinExpr *that) { return visitTree(that); }
    virtual bool visitTopLevel(TopLevel *that) { return visitTree(that); }
    virtual bool visitListDB(ListDB *that) { return visitTree(that); }
    virtual bool visitCreateDBStmt(CreateDBStmt *that) { return visitTree(that); }
    virtual bool visitDropDBStmt(DropDBStmt *that) { return visitTree(that); }
    virtual bool visitUseDBStmt(UseDBStmt *that) { return visitTree(that); }
    virtual bool visitListTB(ListTB *that) { return visitTree(that); }
    virtual bool visitType(Type *that) { return visitTree(that); }
    virtual bool visitField(Field *that) { return visitTree(that); }
    virtual bool visitBoolValue(BoolValue *that) { return visitTree(that); }
    virtual bool visitNullExpr(NullExpr *that) { return visitTree(that); }
    virtual bool visitCompareExpr(CompareExpr *that) { return visitTree(that); }
    virtual bool visitInExpr(InExpr *that) { return visitTree(that); }
    virtual bool visitBetweenExpr(BetweenExpr *that) { return visitTree(that); }
    virtual bool visitComplexExpr(ComplexExpr *that) { return visitTree(that); }
    virtual bool visitCreateTBStmt(CreateTBStmt *that) { return visitTree(that); }
    virtual bool visitDropTBStmt(DropTBStmt *that) { return visitTree(that); }
    virtual bool visitShowTBStmt(ShowTBStmt *that) { return visitTree(that); }
    virtual bool visitColumns(Columns *that) { return visitTree(that); }
    virtual bool visitInsertStmt(InsertStmt *that) { return visitTree(that); }
    virtual bool visitDeleteStmt(DeleteStmt *that) { return visitTree(that); }
    virtual bool visitEq(Eq *that) { return visitTree(that); }
    virtual bool visitUpdateStmt(UpdateStmt *that) { return visitTree(that); }
    virtual bool visitSelector(Selector *that) { return visitTree(that); }
    virtual bool visitSelectors(Selectors *that) { return visitTree(that); }
    virtual bool visitSelectStmt(SelectStmt *that) { return visitTree(that); }
    virtual bool visitCreateIdxStmt(CreateIdxStmt *that) { return visitTree(that); }
    virtual bool visitDropIdxStmt(DropIdxStmt *that) { return visitTree(that); }
    virtual bool visitCValue(CValue *that) { return visitTree(that); }
};

#endif // PARSER_TREE_H_
