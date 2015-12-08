#ifndef PARSER_TREE_H_
#define PARSER_TREE_H_

#include "PrintWriter.h"

#include <string>
#include <vector>

class Tree
{
public:
    virtual void printTo(PrintWriter& pw) = 0;
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

class ShowDBStmt : public Stmt
{
public:
    ShowDBStmt(const char *dbName) : db(std::string(dbName)) {}
    void printTo(PrintWriter& pw);

    std::string db;
};

class ListTB : public Stmt
{
public:
    ListTB() {}
    void printTo(PrintWriter& pw);
};

class CreateTBStmt : public Stmt
{
public:
    CreateTBStmt(const char *tbName) : tb(std::string(tbName)) {}
    void printTo(PrintWriter& pw);

    std::string tb;
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

#endif // PARSER_TREE_H_
