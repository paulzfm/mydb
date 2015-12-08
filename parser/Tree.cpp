#include "Tree.h"

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

void ShowDBStmt::printTo(PrintWriter& pw)
{
    pw.println("Show Database");
    pw.incIndent();
    pw.println(db);
    pw.decIndent();
}

void ListTB::printTo(PrintWriter &pw)
{
    pw.println("List Tables");
}

void CreateTBStmt::printTo(PrintWriter &pw)
{
    pw.println("Create Table");
    pw.incIndent();
    pw.println(tb);
    pw.decIndent();
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
