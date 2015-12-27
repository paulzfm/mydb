#include "execute.h"

bool ExecuteVisitor::visitTopLevel(TopLevel *that)
{
    for (auto& stmt : *that->stmts) {
        msg = "";
        if (!stmt->accept(this)) {
            return false;
        }
    }

    return true;
}

bool ExecuteVisitor::visitListDB(ListDB *that)
{
    return qm->ShowDatabase(msg);
}

bool ExecuteVisitor::visitCreateDBStmt(CreateDBStmt *that)
{
    return qm->CreateDatabase(that->db, msg);
}

bool ExecuteVisitor::visitDropDBStmt(DropDBStmt *that)
{
    return qm->DropDatabase(that->db, msg);
}

bool ExecuteVisitor::visitUseDBStmt(UseDBStmt *that)
{
    return qm->UseDatabase(that->db, msg);
}

bool ExecuteVisitor::visitListTB(ListTB *that)
{
    return qm->ShowTables(msg);
}

bool ExecuteVisitor::visitField(Field *that)
{
    // visit type
    switch (that->type->kind) {
        case Type::TYPE_INT:
            if (that->type->length > 19) {
                msg = "[ERROR] display width out of range for column '" + that->name + "' (max = 19).\n";
                return false;
            }
        case Type::TYPE_STRING:
            if (that->type->length > 65535) {
                msg = "[ERROR] display width out of range for column '" + that->name + "' (max = 65535).\n";
                return false;
            }
    }

    // visit attrs
    for (const auto& attr : *that->attrs) {
        if (attr->kind == Attr::ATTR_AUTO_INCREMENT) {
            if (that->type->kind != Type::TYPE_INT && that->type->kind != Type::TYPE_SMALL_INT &&
                that->type->kind != Type::TYPE_BIG_INT) {
                msg = "[ERROR] incorrect column specifier 'AUTO_INCREMENT' for column '"
                    + that->name + "'.\n";
                return false;
            }
        }
        if (attr->kind == Attr::ATTR_DEFAULT && !attr->def->compatibleWith(that->type)) {
            msg = "[ERROR] Invalid default value for column '" + that->name + "'.\n";
            return false;
        }
    }

    return true;
}

bool ExecuteVisitor::visitCreateTBStmt(CreateTBStmt *that)
{
    PrintWriter pw;
    that->printTo(pw);

    // visit fields
    for (auto& field : *that->fields) {
        if (!field->accept(this)) {
            return false;
        }
    }
    for (int i = 0; i < that->fields->size() - 1; i++) {
        for (int j = i + 1; j < that->fields->size(); j++) {
            if ((*that->fields)[i]->name == (*that->fields)[j]->name) {
                msg = "[ERROR] duplicate column name '" + (*that->fields)[i]->name + "'.\n";
                return false;
            }
        }
    }

    // visit primary key
    if (that->pkeys->size() > 1) {
        msg = "[ERROR] multiple primary key defined.\n";
        return false;
    }
    if (that->pkeys->size() == 1 && !that->exists((*that->pkeys)[0]->key)) {
        msg = "[ERROR] key column '" + (*that->pkeys)[0]->key + "' doesn't exist in table.\n";
        return false;
    }

    // visit foreign keys
    for (auto& key : *that->fkeys) {
        if (!that->exists(key->key)) {
            msg = "[ERROR] key column '" + key->key + "' doesn't exist in table.\n";
            return false;
        }
    }

    if (that->fkeys->size() > 0) {
        for (int i = 0; i < that->fkeys->size() - 1; i++) {
            for (int j = i + 1; j < that->fkeys->size(); j++) {
                if ((*that->fkeys)[i]->key == (*that->fkeys)[j]->key) {
                    msg = "[ERROR] multiple references defined for foreign key '"
                        + (*that->fkeys)[i]->key + "'.\n";
                    return false;
                }
            }
        }
    }

    std::vector<Column> cols;
    int cid = 0;
    int offset = 0;
    for (auto& field : *that->fields) {
        int bytes = field->type->getSize();
        cols.push_back(Column(cid++, field->name, field->type->toDType(), bytes, offset));
        offset += bytes;
    }
    if (offset > 65535) {
        msg = "[ERROR] row size exceeds memory limit (max = 65536 bytes).\n";
        return false;
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
            cons.push_back(Constraint(cid, field->name, attr->kind, data));
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

    return qm->CreateTable(that->tb, cols, cons, msg);
}

bool ExecuteVisitor::visitDropTBStmt(DropTBStmt *that)
{
    return qm->DropTable(that->tb, msg);
}

bool ExecuteVisitor::visitShowTBStmt(ShowTBStmt *that)
{
    return qm->DescTable(that->tb, msg);
}

bool ExecuteVisitor::visitInsertStmt(InsertStmt *that)
{
    PrintWriter pw;
    that->printTo(pw);

    if (that->cols->cols->size() > 0) {
        for (int i = 0; i < that->cols->cols->size() - 1; i++) {
            for (int j = i + 1; j < that->cols->cols->size(); j++) {
                if ((*that->cols->cols)[i] == (*that->cols->cols)[j]) {
                    msg = "[ERROR] column '" + (*that->cols->cols)[i] + "' specified more than once.\n";
                    return false;
                }
            }
        }
    }

    for (auto& values : *that->values) {
        if (that->cols->cols->size() > 0) {
            if (that->cols->cols->size() != values->size()) {
                msg = "[ERROR] column count doesn't match value count.\n";
                return false;
            }

            std::unordered_map<std::string, Value*> data;
            for (int i = 0; i < that->cols->cols->size(); i++) {
                data[(*that->cols->cols)[i]] = (*values)[i];
            }
            if (!qm->Insert(that->tb, data, msg)) {
                return false;
            }
        }

        if (!qm->Insert(that->tb, *values, msg)) {
            return false;
        }
    }

    return true;
}

bool ExecuteVisitor::visitDeleteStmt(DeleteStmt *that)
{
    PrintWriter pw;
    that->printTo(pw);

    return qm->Delete(that->tb, that->where, msg);
}

bool ExecuteVisitor::visitUpdateStmt(UpdateStmt *that)
{
    PrintWriter pw;
    that->printTo(pw);

    std::unordered_map<std::string, Expr*> data;
    for (auto& eq : *that->eqs) {
        data[eq->colName] = eq->expr;
    }

    return qm->Update(that->tb, data, that->where, msg);
}

bool ExecuteVisitor::visitSelector(Selector *that)
{

}

bool ExecuteVisitor::visitSelectors(Selectors *that)
{
    if (!that->all) {
        for (auto& sel : *(that->selectors)) {
            sel->accept(this);
        }
    }
}

bool ExecuteVisitor::visitSelectStmt(SelectStmt *that)
{

}
