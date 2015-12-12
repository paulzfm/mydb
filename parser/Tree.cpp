#include "Tree.h"

void Value::printTo(PrintWriter &pw)
{
    pw.print(val);
    pw.print(" ");
    switch (kind) {
        case VALUE_INT: pw.print("integer");
        case VALUE_REAL: pw.print("real");
        case VALUE_STRING: pw.print("string");
    }
}

void TopLevel::printTo(PrintWriter& pw)
{
    pw.println("Top Level");
    pw.incIndent();
    for (auto stmt : *stmts) {
        stmt->printTo(pw);
    }
    pw.decIndent();
    pw.println("done");
}

void ListDB::printTo(PrintWriter &pw)
{
    pw.println("List Databases");
}

void CreateDBStmt::printTo(PrintWriter& pw)
{
    pw.println("Create Database");
    pw.incIndent();
    pw.println(db);
    pw.decIndent();
}

void DropDBStmt::printTo(PrintWriter& pw)
{
    pw.println("Drop Database");
    pw.incIndent();
    pw.println(db);
    pw.decIndent();
}

void UseDBStmt::printTo(PrintWriter& pw)
{
    pw.println("Use Database");
    pw.incIndent();
    pw.println(db);
    pw.decIndent();
}

void ListTB::printTo(PrintWriter &pw)
{
    pw.println("List Tables");
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

void Field::printTo(PrintWriter &pw)
{
    pw.printIndent();
    type->printTo(pw);
    pw.print(" ");
    pw.print(name);
    for (auto& attr : *attrs) {
        pw.print(" ");
        switch (attr) {
            case ATTR_AUTO_INCREMENT: pw.print("auto increment"); break;
            case ATTR_NOT_NULL: pw.print("not null"); break;
            case ATTR_UNIQUE: pw.print("unique"); break;
        }
    }
    pw.println("");
}

void UnonExpr::printTo(PrintWriter &pw)
{
    switch (op) {
        case OP_IS_NULL: pw.println("is null"); break;
        case OP_IS_NOT_NULL: pw.println("is not null"); break;
    }

    pw.incIndent();
    pw.println(name);
    pw.decIndent();
}

void BinExpr::printTo(PrintWriter &pw)
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
    pw.println(left);
    right->printTo(pw);
    pw.println("");
    pw.decIndent();
}

void InExpr::printTo(PrintWriter &pw)
{
    switch (op) {
        case OP_IN: pw.println("in"); break;
        case OP_NOT_IN: pw.println("not in"); break;
    }

    pw.incIndent();
    pw.println(left);
    pw.print("{ ");
    for (auto& val : *right) {
        val->printTo(pw);
        pw.print(" ");
    }
    pw.println("}");
    pw.decIndent();
}

void BetweenExpr::printTo(PrintWriter &pw)
{
    switch (op) {
        case OP_BETWEEN: pw.println("between"); break;
        case OP_NOT_BETWEEN: pw.println("not between"); break;
    }

    pw.incIndent();
    pw.println(left);
    pw.print("(");
    rightL->printTo(pw);
    pw.print(", ");
    rightR->printTo(pw);
    pw.println(")");
    pw.decIndent();
}

void ComExpr::printTo(PrintWriter &pw)
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
    if (!empty) {
        pw.println("check");
        pw.incIndent();
        check->printTo(pw);
        pw.decIndent();
    } else {
        pw.println("no check");
    }
}

void PrimaryKey::printTo(PrintWriter &pw)
{
    if (!empty) {
        pw.println("primary key");
        pw.incIndent();
        pw.println(key);
        pw.decIndent();
    } else {
        pw.println("no primary key");
    }
}

void CreateTBStmt::printTo(PrintWriter &pw)
{
    pw.println("Create Table");
    pw.incIndent();
    pw.println(tb);
    pw.println("fields");
    pw.incIndent();
    for (auto& field : *fields) {
        field->printTo(pw);
    }
    pw.decIndent();
    check->printTo(pw);
    pkey->printTo(pw);
}

void DropTBStmt::printTo(PrintWriter &pw)
{
    pw.println("Drop Table");
    pw.incIndent();
    pw.println(tb);
    pw.decIndent();
}

void ShowTBStmt::printTo(PrintWriter &pw)
{
    pw.println("Show Table");
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
    pw.println(tbName);
    cols->printTo(pw);
    pw.println("values");
    pw.incIndent();
    for (auto& val : *values) {
        pw.printIndent();
        val->printTo(pw);
        pw.println("");
    }
    pw.decIndent();
    pw.decIndent();
}

void DeleteStmt::printTo(PrintWriter &pw)
{
    pw.println("delete");
    pw.incIndent();
    pw.println(tbName);
    pw.println("where");
    pw.incIndent();
    where->printTo(pw);
    pw.decIndent();
    pw.decIndent();
}

void Eq::printTo(PrintWriter &pw)
{
    pw.printIndent();
    pw.print(colName);
    pw.print(" ");
    value->printTo(pw);
    pw.println("");
}

void UpdateStmt::printTo(PrintWriter &pw)
{
    pw.println("update");
    pw.incIndent();
    pw.println(tbName);
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
    if (tbName != "") { // <tbName> . <colName>
        pw.println(tbName + "." + colName);
    } else {
        switch (func) {
            case FUNC_NULL: // <colName>
                pw.println(colName);
                break;
            case FUNC_SUM:  // SUM ( <colName> )
                pw.println("SUM(" + colName + ")");
                break;
            case FUNC_AVG:  // AVG ( <colName> )
                pw.println("AVG(" + colName + ")");
                break;
            case FUNC_MAX:  // MAX ( <colName> )
                pw.println("MAX(" + colName + ")");
                break;
            case FUNC_MIN:  // MIN ( <colName> )
                pw.println("MIN(" + colName + ")");
                break;
        }
    }
}

void SelectStmt::printTo(PrintWriter &pw)
{
    pw.println("select");
    pw.incIndent();
    pw.println(tbName);
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
        pw.println(gb->colName);
        pw.decIndent();
    }
}
