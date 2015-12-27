#ifndef PARSER_EXECUTE_H_
#define PARSER_EXECUTE_H_

#include <stdio.h>

#include "../parser/Tree.h"
#include "../query/QueryManager.h"

class ExecuteVisitor : public Visitor
{
public:
    ExecuteVisitor(QueryManager *qm) : qm(qm) {}

    bool visitTopLevel(TopLevel *that);
    bool visitListDB(ListDB *that);
    bool visitCreateDBStmt(CreateDBStmt *that);
    bool visitDropDBStmt(DropDBStmt *that);
    bool visitUseDBStmt(UseDBStmt *that);
    bool visitListTB(ListTB *that);
    bool visitField(Field *that);
    bool visitCreateTBStmt(CreateTBStmt *that);
    bool visitDropTBStmt(DropTBStmt *that);
    bool visitShowTBStmt(ShowTBStmt *that);
    bool visitInsertStmt(InsertStmt *that);
    bool visitDeleteStmt(DeleteStmt *that);
    bool visitUpdateStmt(UpdateStmt *that);
    bool visitSelector(Selector *that);
    bool visitSelectors(Selectors *that);
    bool visitSelectStmt(SelectStmt *that);

    std::string msg; // to save return message

private:
    QueryManager *qm;
};

#endif // PARSER_EXECUTE_H_
