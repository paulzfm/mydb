#ifndef PARSER_EXECUTE_H_
#define PARSER_EXECUTE_H_

#include <stdio.h>

#include "../parser/Tree.h"
#include "../query/QueryManager.h"

class ExecuteVisitor : public Visitor
{
public:
    ExecuteVisitor(QueryManager *qm) : qm(qm) {}

    bool visitValue(Value *that);
    bool visitUnonExpr(UnonExpr *that);
    bool visitBinExpr(BinExpr *that);
    bool visitTopLevel(TopLevel *that);
    bool visitListDB(ListDB *that);
    bool visitCreateDBStmt(CreateDBStmt *that);
    bool visitDropDBStmt(DropDBStmt *that);
    bool visitUseDBStmt(UseDBStmt *that);
    bool visitListTB(ListTB *that);
    bool visitType(Type *that);
    bool visitField(Field *that);
    bool visitNullExpr(NullExpr *that);
    bool visitCompareExpr(CompareExpr *that);
    bool visitInExpr(InExpr *that);
    bool visitBetweenExpr(BetweenExpr *that);
    bool visitComplexExpr(ComplexExpr *that);
    bool visitPrimaryKey(PrimaryKey *that);
    bool visitCreateTBStmt(CreateTBStmt *that);
    bool visitDropTBStmt(DropTBStmt *that);
    bool visitShowTBStmt(ShowTBStmt *that);
    bool visitColumns(Columns *that);
    bool visitInsertStmt(InsertStmt *that);
    bool visitDeleteStmt(DeleteStmt *that);
    bool visitEq(Eq *that);
    bool visitUpdateStmt(UpdateStmt *that);
    bool visitSelector(Selector *that);
    bool visitSelectors(Selectors *that);
    bool visitWhere(Where *that);
    bool visitGroupBy(GroupBy *that);
    bool visitSelectStmt(SelectStmt *that);

    std::string msg; // to save return message

private:
    QueryManager *qm;
};

#endif // PARSER_EXECUTE_H_
