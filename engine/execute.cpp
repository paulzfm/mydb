#include "execute.h"

void ExecuteVisitor::visitValue(Value *that)
{
}

void ExecuteVisitor::visitUnonExpr(UnonExpr *that)
{
    that->expr->accept(this);
}

void ExecuteVisitor::visitBinExpr(BinExpr *that)
{
    that->left->accept(this);
    that->right->accept(this);
}

void ExecuteVisitor::visitTopLevel(TopLevel *that)
{
    for (auto& stmt : *(that->stmts)) {
        stmt->accept(this);
    }
}

void ExecuteVisitor::visitListDB(ListDB *that)
{
    qm->ShowDatabase(msg);
}

void ExecuteVisitor::visitCreateDBStmt(CreateDBStmt *that)
{
    qm->CreateDatabase(that->db, msg);
}

void ExecuteVisitor::visitDropDBStmt(DropDBStmt *that)
{
    qm->DropDatabase(that->db, msg);
}

void ExecuteVisitor::visitUseDBStmt(UseDBStmt *that)
{
    qm->UseDatabase(that->db, msg);
}

void ExecuteVisitor::visitListTB(ListTB *that)
{
    qm->ShowTables(msg);
}

void ExecuteVisitor::visitType(Type *that)
{

}

void ExecuteVisitor::visitField(Field *that)
{
    that->type->accept(this);
}

void ExecuteVisitor::visitNullExpr(NullExpr *that)
{

}

void ExecuteVisitor::visitCompareExpr(CompareExpr *that)
{
    that->right->accept(this);
}

void ExecuteVisitor::visitInExpr(InExpr *that)
{
    for (auto& val : *(that->right)) {
        val->accept(this);
    }
}

void ExecuteVisitor::visitBetweenExpr(BetweenExpr *that)
{
    that->rightL->accept(this);
    that->rightR->accept(this);
}

void ExecuteVisitor::visitComplexExpr(ComplexExpr *that)
{
    that->left->accept(this);
    that->right->accept(this);
}

void ExecuteVisitor::visitCreateTBStmt(CreateTBStmt *that)
{
    for (auto& field : *(that->fields)) {
        field->accept(this);
    }
    if (!that->check->empty) {
        that->check->check->accept(this);
    }
    if (!that->pkey->empty) {

    }
    // qm->CreateTable(that->tb, cols);
}

void ExecuteVisitor::visitDropTBStmt(DropTBStmt *that)
{
    qm->DropTable(that->tb, msg);
}

void ExecuteVisitor::visitShowTBStmt(ShowTBStmt *that)
{
    qm->DescTable(that->tb, msg);
}

void ExecuteVisitor::visitColumns(Columns *that)
{

}

void ExecuteVisitor::visitInsertStmt(InsertStmt *that)
{

}

void ExecuteVisitor::visitDeleteStmt(DeleteStmt *that)
{
    that->where->accept(this);
    // qm->Delete(that->tb, that->where);
}

void ExecuteVisitor::visitEq(Eq *that)
{
    that->expr->accept(this);
}

void ExecuteVisitor::visitUpdateStmt(UpdateStmt *that)
{
    for (auto& eq : *(that->eqs)) {
        eq->accept(this);
    }
    that->where->accept(this);
    // qm->Update(that->tb, unordered_map<string, Expr *> data, Expr *expr);
}

void ExecuteVisitor::visitSelector(Selector *that)
{

}

void ExecuteVisitor::visitSelectors(Selectors *that)
{
    if (!that->all) {
        for (auto& sel : *(that->selectors)) {
            sel->accept(this);
        }
    }
}

void ExecuteVisitor::visitSelectStmt(SelectStmt *that)
{
    that->sel->accept(this);
    if (!that->where->empty) {
        that->where->where->accept(this);
    }
    if (!that->gb->empty) {

    }

    // qm->Select(that->tb, vector<string> attrs, Expr *expr)
}
