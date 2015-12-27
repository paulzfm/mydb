#include "Tree.h"

void Value::printTo(PrintWriter &pw)
{
    switch (kind) {
        case VALUE_INT: pw.println(val + " integer"); break;
        case VALUE_REAL: pw.println(val + " real"); break;
        case VALUE_STRING: pw.println(val + " string"); break;
        case VALUE_NULL: pw.println("value null"); break;
    }
}

bool Value::compatibleWith(Type *type)
{
    switch (kind) {
        case VALUE_NULL:
            return true;
        case VALUE_INT:
            return type->kind == Type::TYPE_BIG_INT || type->kind == Type::TYPE_INT
                || type->kind == Type::TYPE_SMALL_INT;
        case VALUE_REAL:
            return type->kind == Type::TYPE_FLOAT || type->kind == Type::TYPE_DOUBLE;
        case VALUE_STRING:
            if (val.length() == 1) {
                return type->kind == Type::TYPE_STRING || type->kind == Type::TYPE_CHAR;
            } else {
                return type->kind == Type::TYPE_STRING;
            }
        case VALUE_TRUE:
            return type->kind == Type::TYPE_BOOLEAN;
        case VALUE_FALSE:
            return type->kind == Type::TYPE_BOOLEAN;
        default:
            return false;
    }
}

void Col::printTo(PrintWriter &pw)
{
    pw.println(tb + "." + col);
}

void UnonExpr::printTo(PrintWriter &pw)
{
    switch (op) {
        case OP_POS: pw.println("pos"); break;
        case OP_NEG: pw.println("neg"); break;
    }

    pw.incIndent();
    expr->printTo(pw);
    pw.decIndent();
}

void BinExpr::printTo(PrintWriter &pw)
{
    switch (op) {
        case OP_ADD: pw.println("add"); break;
        case OP_SUB: pw.println("sub"); break;
        case OP_MUL: pw.println("mul"); break;
        case OP_DIV: pw.println("div"); break;
        case OP_MOD: pw.println("mod"); break;
    }

    pw.incIndent();
    left->printTo(pw);
    right->printTo(pw);
    pw.decIndent();
}

void TopLevel::printTo(PrintWriter& pw)
{
    pw.println("top level");
    pw.incIndent();
    for (auto stmt : *stmts) {
        stmt->printTo(pw);
    }
    pw.decIndent();
    pw.println("done");
}

void ListDB::printTo(PrintWriter &pw)
{
    pw.println("list databases");
}

void CreateDBStmt::printTo(PrintWriter& pw)
{
    pw.println("create database");
    pw.incIndent();
    pw.println(db);
    pw.decIndent();
}

void DropDBStmt::printTo(PrintWriter& pw)
{
    pw.println("drop database");
    pw.incIndent();
    pw.println(db);
    pw.decIndent();
}

void UseDBStmt::printTo(PrintWriter& pw)
{
    pw.println("use database");
    pw.incIndent();
    pw.println(db);
    pw.decIndent();
}

void ListTB::printTo(PrintWriter &pw)
{
    pw.println("list tables");
}

void Type::printTo(PrintWriter &pw)
{
    switch (kind) {
        case TYPE_INT: printf("int(%d)", length); break;
        case TYPE_SMALL_INT: pw.print("small int"); break;
        case TYPE_BIG_INT: pw.print("big int"); break;
        case TYPE_FLOAT: pw.print("float"); break;
        case TYPE_DOUBLE: pw.print("double"); break;
        case TYPE_STRING: printf("string(%d)", length); break;
        case TYPE_CHAR: pw.print("char"); break;
        case TYPE_BOOLEAN: pw.print("boolean"); break;
        case TYPE_DATETIME: pw.print("datetime"); break;
        default: ;
    }
}

char Type::toDType()
{
    switch (kind) {
        case TYPE_INT:
            if (length < 5) return DType::SHORT;
            else if (length < 10) return DType::INT;
            else return DType::LONG;
        case TYPE_SMALL_INT: return DType::SHORT;
        case TYPE_BIG_INT: return DType::LONG;
        case TYPE_FLOAT: return DType::FLOAT;
        case TYPE_DOUBLE: return DType::DOUBLE;
        case TYPE_STRING: return DType::STRING;
        case TYPE_CHAR: return DType::CHAR;
        case TYPE_BOOLEAN: return DType::BOOL;
    }
}

int Type::getSize()
{
    switch (kind) {
        case TYPE_INT:
            if (length < 5) return 2;
            else if (length < 10) return 4;
            else return 8;
        case TYPE_SMALL_INT: return 2;
        case TYPE_BIG_INT: return 8;
        case TYPE_FLOAT: return 4;
        case TYPE_DOUBLE: return 8;
        case TYPE_STRING: return length;
        case TYPE_CHAR: return 1;
        case TYPE_BOOLEAN: return 1;
    }
}

void Field::printTo(PrintWriter &pw)
{
    pw.printIndent();
    type->printTo(pw);
    pw.print(" ");
    pw.print(name);
    for (auto& attr : *attrs) {
        pw.print(" ");
        switch (attr->kind) {
            case Attr::ATTR_AUTO_INCREMENT: pw.print("auto increment"); break;
            case Attr::ATTR_NOT_NULL: pw.print("not null"); break;
            case Attr::ATTR_UNIQUE: pw.print("unique"); break;
            case Attr::ATTR_DEFAULT:
                pw.print("default ");
                attr->def->printTo(pw);
                break;
        }
    }
    pw.println("");
}

void NullExpr::printTo(PrintWriter &pw)
{
    switch (op) {
        case OP_IS_NULL: pw.println("is null"); break;
        case OP_IS_NOT_NULL: pw.println("is not null"); break;
    }

    pw.incIndent();
    pw.println(name->tb + "." + name->col);
    pw.decIndent();
}

void CompareExpr::printTo(PrintWriter &pw)
{
    switch (op) {
        case OP_EQ: pw.println("="); break;
        case OP_NE: pw.println("<>"); break;
        case OP_GE: pw.println(">="); break;
        case OP_LE: pw.println("<="); break;
        case OP_GT: pw.println(">"); break;
        case OP_LT: pw.println("<"); break;
        case OP_LIKE: pw.println("like"); break;
        case OP_NOT_LIKE: pw.println("not like"); break;
    }

    pw.incIndent();
    pw.println(left->tb + "." + left->col);
    right->printTo(pw);
    pw.decIndent();
}

void InExpr::printTo(PrintWriter &pw)
{
    switch (op) {
        case OP_IN: pw.println("in"); break;
        case OP_NOT_IN: pw.println("not in"); break;
    }

    pw.incIndent();
    pw.println(left->tb + "." + left->col);
    pw.println("set of");
    pw.incIndent();
    for (auto& val : *right) {
        val->printTo(pw);
    }
    pw.decIndent();
    pw.decIndent();
}

void BetweenExpr::printTo(PrintWriter &pw)
{
    switch (op) {
        case OP_BETWEEN: pw.println("between"); break;
        case OP_NOT_BETWEEN: pw.println("not between"); break;
    }

    pw.incIndent();
    pw.println(left->tb + "." + left->col);
    pw.println("range within");
    pw.incIndent();
    rightL->printTo(pw);
    rightR->printTo(pw);
    pw.decIndent();
    pw.decIndent();
}

void ComplexExpr::printTo(PrintWriter &pw)
{
    switch (op) {
        case OP_AND: pw.println("and"); break;
        case OP_OR: pw.println("or"); break;
    }

    pw.incIndent();
    left->printTo(pw);
    right->printTo(pw);
    pw.decIndent();
}

void Check::printTo(PrintWriter &pw)
{
    pw.println("check");
    pw.incIndent();
    check->printTo(pw);
    pw.decIndent();
}

void PrimaryKey::printTo(PrintWriter &pw)
{
    pw.println("primary key");
    pw.incIndent();
    pw.println(key);
    pw.decIndent();
}

void ForeignKey::printTo(PrintWriter &pw)
{
    pw.println("foreign key");
    pw.incIndent();
    pw.println(key);
    pw.println("references to");
    pw.incIndent();
    pw.println(rtb);
    pw.println(rkey);
    pw.decIndent();
    pw.decIndent();

}

void CreateTBStmt::printTo(PrintWriter &pw)
{
    pw.println("create table");
    pw.incIndent();
    pw.println(tb);
    pw.println("fields");
    pw.incIndent();
    for (auto& field : *fields) {
        field->printTo(pw);
    }
    pw.decIndent();
    for (auto& check : *checks) {
        check->printTo(pw);
    }
    for (auto& pkey : *pkeys) {
        pkey->printTo(pw);
    }
    for (auto& key : *fkeys) {
        key->printTo(pw);
    }
    pw.decIndent();
}

bool CreateTBStmt::exists(std::string &col)
{
    for (auto& field : *fields) {
        if (field->name == col) {
            return true;
        }
    }

    return false;
}

int CreateTBStmt::indexOf(std::string &col)
{
    for (int i = 0; i < fields->size(); i++) {
        if ((*fields)[i]->name == col) {
            return i;
        }
    }

    return -1;
}

void DropTBStmt::printTo(PrintWriter &pw)
{
    pw.println("drop table");
    pw.incIndent();
    pw.println(tb);
    pw.decIndent();
}

void ShowTBStmt::printTo(PrintWriter &pw)
{
    pw.println("show table");
    pw.incIndent();
    pw.println(tb);
    pw.decIndent();
}

void Columns::printTo(PrintWriter &pw)
{
    if (cols->size() == 0) {
        pw.println("default order");
    } else {
        pw.println("order");
        pw.incIndent();
        for (auto& col : *cols) {
            pw.println(col);
        }
        pw.decIndent();
    }
}

void InsertStmt::printTo(PrintWriter &pw)
{
    pw.println("insert");
    pw.incIndent();
    pw.println(tb);
    cols->printTo(pw);
    for (auto& set : *values) {
        pw.println("values");
        pw.incIndent();
        for (auto& val : *set) {
            val->printTo(pw);
        }
        pw.decIndent();
    }
    pw.decIndent();
}

void DeleteStmt::printTo(PrintWriter &pw)
{
    pw.println("delete");
    pw.incIndent();
    pw.println(tb);
    pw.println("where");
    pw.incIndent();
    where->printTo(pw);
    pw.decIndent();
    pw.decIndent();
}

void Eq::printTo(PrintWriter &pw)
{
    pw.println(colName + " =");
    pw.incIndent();
    expr->printTo(pw);
    pw.decIndent();
}

void UpdateStmt::printTo(PrintWriter &pw)
{
    pw.println("update");
    pw.incIndent();
    pw.println(tb);
    pw.println("set");
    pw.incIndent();
    for (auto& eq : *eqs) {
        eq->printTo(pw);
    }
    pw.decIndent();
    pw.println("where");
    pw.incIndent();
    where->printTo(pw);
    pw.decIndent();
    pw.decIndent();
}

void Selectors::printTo(PrintWriter &pw)
{
    if (all) {
        pw.println("all");
    } else {
        for (auto& selector : *selectors) {
            selector->printTo(pw);
        }
    }
}

void Selector::printTo(PrintWriter &pw)
{
    switch (func) {
        case FUNC_NULL:
            col->printTo(pw);
            break;
        case FUNC_SUM:
            pw.println("sum");
            pw.incIndent();
            col->printTo(pw);
            pw.decIndent();
            break;
        case FUNC_AVG:
            pw.println("avg");
            pw.incIndent();
            col->printTo(pw);
            pw.decIndent();
            break;
        case FUNC_MAX:
            pw.println("max");
            pw.incIndent();
            col->printTo(pw);
            pw.decIndent();
            break;
        case FUNC_MIN:
            pw.println("min");
            pw.incIndent();
            col->printTo(pw);
            pw.decIndent();
            break;
    }
}

void SelectStmt::printTo(PrintWriter &pw)
{
    pw.println("select");
    pw.incIndent();
    for (auto& tb : tbs) {
        pw.println(tb);
    }
    pw.println("selectors");
    pw.incIndent();
    sel->printTo(pw);
    pw.decIndent();
    if (!where->empty) {
        pw.println("where");
        pw.incIndent();
        where->where->printTo(pw);
        pw.decIndent();
    }
    if (!gb->empty) {
        pw.println("group by");
        pw.incIndent();
        pw.println(gb->tb);
        pw.println(gb->col);
        pw.decIndent();
    }
    pw.decIndent();
}

void CreateIdxStmt::printTo(PrintWriter &pw)
{
    pw.println("create index");
    pw.incIndent();
    pw.println(tb);
    pw.println(col);
    pw.decIndent();
}

void DropIdxStmt::printTo(PrintWriter &pw)
{
    pw.println("drop index");
    pw.incIndent();
    pw.println(tb);
    pw.println(col);
    pw.decIndent();
}

bool Value::accept(Visitor *v)
{
    return v->visitValue(this);
}

bool Col::accept(Visitor *v)
{
    return v->visitCol(this);
}

bool UnonExpr::accept(Visitor *v)
{
    return v->visitUnonExpr(this);
}

bool BinExpr::accept(Visitor *v)
{
    return v->visitBinExpr(this);
}

bool TopLevel::accept(Visitor *v)
{
    return v->visitTopLevel(this);
}

bool ListDB::accept(Visitor *v)
{
    return v->visitListDB(this);
}

bool CreateDBStmt::accept(Visitor *v)
{
    return v->visitCreateDBStmt(this);
}

bool DropDBStmt::accept(Visitor *v)
{
    return v->visitDropDBStmt(this);
}

bool UseDBStmt::accept(Visitor *v)
{
    return v->visitUseDBStmt(this);
}

bool ListTB::accept(Visitor *v)
{
    return v->visitListTB(this);
}

bool Type::accept(Visitor *v)
{
    return v->visitType(this);
}

bool Field::accept(Visitor *v)
{
    return v->visitField(this);
}

bool NullExpr::accept(Visitor *v)
{
    return v->visitNullExpr(this);
}

bool CompareExpr::accept(Visitor *v)
{
    return v->visitCompareExpr(this);
}

bool InExpr::accept(Visitor *v)
{
    return v->visitInExpr(this);
}

bool BetweenExpr::accept(Visitor *v)
{
    return v->visitBetweenExpr(this);
}

bool ComplexExpr::accept(Visitor *v)
{
    return v->visitComplexExpr(this);
}

bool CreateTBStmt::accept(Visitor *v)
{
    return v->visitCreateTBStmt(this);
}

bool DropTBStmt::accept(Visitor *v)
{
    return v->visitDropTBStmt(this);
}

bool ShowTBStmt::accept(Visitor *v)
{
    return v->visitShowTBStmt(this);
}

bool Columns::accept(Visitor *v)
{
    return v->visitColumns(this);
}

bool InsertStmt::accept(Visitor *v)
{
    return v->visitInsertStmt(this);
}

bool DeleteStmt::accept(Visitor *v)
{
    return v->visitDeleteStmt(this);
}

bool Eq::accept(Visitor *v)
{
    return v->visitEq(this);
}

bool UpdateStmt::accept(Visitor *v)
{
    return v->visitUpdateStmt(this);
}

bool Selector::accept(Visitor *v)
{
    return v->visitSelector(this);
}

bool Selectors::accept(Visitor *v)
{
    return v->visitSelectors(this);
}

bool SelectStmt::accept(Visitor *v)
{
    return v->visitSelectStmt(this);
}

bool CreateIdxStmt::accept(Visitor *v)
{
    return v->visitCreateIdxStmt(this);
}

bool DropIdxStmt::accept(Visitor *v)
{
    return v->visitDropIdxStmt(this);
}
