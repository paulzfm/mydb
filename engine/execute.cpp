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
    for (auto& stmt : *that->stmts) {
        msg = "";
        success = false;
        stmt->accept(this);
        if (!success) {
            break;
        }
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
    // visit type
    switch (that->type->kind) {
        case Type::TYPE_INT:
            if (that->type->length > 19) {
                msg = "[ERROR] display width out of range for column '" + that->name + "' (max = 19).\n";
                return;
            }
            break;
        case Type::TYPE_STRING:
            if (that->type->length > 65535) {
                msg = "[ERROR] display width out of range for column '" + that->name + "' (max = 65535).\n";
                return;
            }
            break;
    }

    // visit attrs
    for (const auto& attr : *that->attrs) {
        if (attr == Field::ATTR_AUTO_INCREMENT) {
            if (that->type->kind != Type::TYPE_INT && that->type->kind != Type::TYPE_SMALL_INT &&
                that->type->kind != Type::TYPE_BIG_INT) {
                msg = "[ERROR] incorrect column specifier 'AUTO_INCREMENT' for column '"
                    + that->name + "'.\n";
                return;
            }
        }
    }
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
    PrintWriter pw;
    that->printTo(pw);

    // visit fields
    for (auto& field : *that->fields) {
        field->accept(this);
    }
    for (int i = 0; i < that->fields->size() - 1; i++) {
        for (int j = i + 1; j < that->fields->size(); j++) {
            if ((*that->fields)[i]->name == (*that->fields)[j]->name) {
                msg = "[ERROR] duplicate column name '" + (*that->fields)[i]->name + "'.\n";
                return;
            }
        }
    }

    // visit primary key
    if (that->pkeys->size() > 1) {
        msg = "[ERROR] multiple primary key defined.\n";
        return;
    }
    if (that->pkeys->size() == 1 && !that->exists((*that->pkeys)[0]->key)) {
        msg = "[ERROR] key column '" + (*that->pkeys)[0]->key + "' doesn't exist in table.\n";
        return;
    }

    // visit foreign keys
    for (auto& key : *that->fkeys) {
        if (!that->exists(key->key)) {
            msg = "[ERROR] key column '" + key->key + "' doesn't exist in table.\n";
            return;
        }
    }

    if (that->fkeys->size() > 0) {
        for (int i = 0; i < that->fkeys->size() - 1; i++) {
            for (int j = i + 1; j < that->fkeys->size(); j++) {
                if ((*that->fkeys)[i]->key == (*that->fkeys)[j]->key) {
                    msg = "[ERROR] multiple references defined for foreign key '"
                        + (*that->fkeys)[i]->key + "'.\n";
                    return;
                }
            }
        }
    }

    std::vector<Column> cols;
    int cid = 0;
    int offset = 0;
    for (auto& field : *that->fields) {
        short bytes = field->type->getSize();
        cols.push_back(Column(cid++, field->name, field->type->toDType(), bytes, offset));
        offset += bytes;
    }
    if (offset > 65535) {
        msg = "[ERROR] row size exceeds memory limit (max = 65536 bytes).\n";
        return;
    }

    rapidjson::Value data;
    std::vector<Constraint> cons;
    if (that->pkeys->size() == 1) {
        std::string key = (*that->pkeys)[0]->key;
        cons.push_back(Constraint(that->indexOf(key), key,
            Constraint::PRIMARY_KEY, data));
    }
    for (auto& key : *that->fkeys) {
        cons.push_back(Constraint(that->indexOf(key->key), key->key,
            Constraint::FOREIGN_KEY, data));
    }
    cid = 0;
    for (auto& field : *that->fields) {
        for (auto& attr : *field->attrs) {
            cons.push_back(Constraint(cid, field->name, attr, data));
        }
        cid++;
    }

    for (const auto& col : cols) {
        std::cout << "cid=" << col.cid << ",name=" << col.name << ",type=" << int(col.type)
            << ",size=" << col.size << ",offset=" << col.offset << "\n";
    }
    for (const auto& con : cons) {
        std::cout << "cid=" << con.cid << ",name=" << con.name << ",type=" << int(con.type) << "\n";
    }
    success = qm->CreateTable(that->tb, cols, cons, msg);
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
