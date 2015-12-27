#ifndef PARSER_EXECUTE_H_
#define PARSER_EXECUTE_H_

#include <stdio.h>

#include "../parser/Tree.h"
#include "../query/QueryManager.h"

class ExecuteVisitor : public Visitor
{
public:
    ExecuteVisitor(QueryManager *qm) : qm(qm) {}

    void visitValue(Value *that);
    void visitUnonExpr(UnonExpr *that);
    void visitBinExpr(BinExpr *that);
    void visitTopLevel(TopLevel *that);
    void visitListDB(ListDB *that);
    void visitCreateDBStmt(CreateDBStmt *that);
    void visitDropDBStmt(DropDBStmt *that);
    void visitUseDBStmt(UseDBStmt *that);
    void visitListTB(ListTB *that);
    void visitType(Type *that);
    void visitField(Field *that);
    void visitNullExpr(NullExpr *that);
    void visitCompareExpr(CompareExpr *that);
    void visitInExpr(InExpr *that);
    void visitBetweenExpr(BetweenExpr *that);
    void visitComplexExpr(ComplexExpr *that);
    void visitPrimaryKey(PrimaryKey *that);
    void visitCreateTBStmt(CreateTBStmt *that);
    void visitDropTBStmt(DropTBStmt *that);
    void visitShowTBStmt(ShowTBStmt *that);
    void visitColumns(Columns *that);
    void visitInsertStmt(InsertStmt *that);
    void visitDeleteStmt(DeleteStmt *that);
    void visitEq(Eq *that);
    void visitUpdateStmt(UpdateStmt *that);
    void visitSelector(Selector *that);
    void visitSelectors(Selectors *that);
    void visitWhere(Where *that);
    void visitGroupBy(GroupBy *that);
    void visitSelectStmt(SelectStmt *that);

    std::string msg; // to save return message
    bool success; // whether a statement execution is successful

private:
    QueryManager *qm;
};

#endif // PARSER_EXECUTE_H_
