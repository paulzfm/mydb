#include "Evaluator.h"

Evaluator::Evaluator(QueryManager* qm, const char* rec) : qm(qm), rec(rec){
}

vector<DValue>& Evaluator::getValues() {
    return values;
}

void Evaluator::visitCol(Col *that) {
    // TODO
    DValue val;
    values.push_back(val);
}

void Evaluator::visitValue(Value *that) {
    switch (that->kind) {
        case Value::VALUE_INT:
            values.push_back(DValue((int64_t)atoll(that->val.c_str())));
        case Value::VALUE_REAL:
            values.push_back(DValue(atof(that->val.c_str())));
        case Value::VALUE_STRING:
            values.push_back(DValue(that->val.c_str()));
        case Value::VALUE_NULL:
            values.push_back(DValue());
    }
}

void Evaluator::visitUnonExpr(UnonExpr *that) {
    that->expr->accept(this);
    DValue val = values.back();
    values.pop_back();
    if (that->op == Expr::OP_NEG) {
        assert(val.isNum());
        if (val.isInt()) val = DValue(-val.getInt());
        if (val.isReal()) val = DValue(-val.getReal());
    }
    values.push_back(val);
}

void Evaluator::visitBinExpr(BinExpr *that) {
    that->left->accept(this);
    that->right->accept(this);

    DValue a = values.back();
    assert(a.isNum());
    values.pop_back();
    DValue b = values.back();
    assert(b.isNum());
    values.pop_back();
    DValue val;
    switch (that->op) {
        case Expr::OP_ADD:
            val = a + b;
            break;
        case Expr::OP_SUB:
            val = a - b;
            break;
        case Expr::OP_MUL:
            val = a * b;
            break;
        case Expr::OP_DIV:
            val = a / b;
            break;
        case Expr::OP_MOD:
            assert(a.isInt() && b.isInt());
            val = a % b;
            break;
    }
    values.push_back(val);
}

void Evaluator::visitNullExpr(NullExpr *that) {
    // TODO: get null column value and visiting Column
    char* null;
    Column col;
    bool res = (null[col.cid / 8] & (1 << (col.cid % 8)));
    values.push_back(DValue(res != (that->op == BoolExpr::OP_IS_NOT_NULL)));
}

void Evaluator::visitCompareExpr(CompareExpr *that) {
    // TODO
    DValue val;
    that->right->accept(this);

    DValue res;
    switch (that->op) {
        case BoolExpr::OP_EQ:
            res = (val == values.back());
            break;
        case BoolExpr::OP_NE:
            res = (val != values.back());
            break;
        case BoolExpr::OP_GE:
            res = (val >= values.back());
            break;
        case BoolExpr::OP_LE:
            res = (val <= values.back());
            break;
        case BoolExpr::OP_GT:
            res = (val > values.back());
            break;
        case BoolExpr::OP_LT:
            res = (val < values.back());
            break;
    }
    values.pop_back();
    values.push_back(res);
}

void Evaluator::visitInExpr(InExpr *that) {
    // TODO
    DValue val;
    bool res = false;
    for (Value* e : *that->right) {
        if ((DValue(e) == val).getBool()) {
            res = true;
            break;
        }
    }
    values.push_back(DValue(res != (that->op == BoolExpr::OP_NOT_IN)));
}

void Evaluator::visitBetweenExpr(BetweenExpr *that) {
    int low = atoll(that->rightL->val.c_str()),
        high = atoll(that->rightR->val.c_str());
    // TODO: get column value
    DValue val;
    assert(val.isNum());
    bool res;
    if (val.isInt())
        res = low <= val.getInt() && val.getInt() <= high;
    else
        res = low <= val.getReal() && val.getReal() <= high;
    values.push_back(DValue(res != (that->op == BoolExpr::OP_NOT_BETWEEN)));
}

void Evaluator::visitComplexExpr(ComplexExpr *that) {
    that->left->accept(this);
    that->right->accept(this);

    DValue a = values.back();
    values.pop_back();
    DValue b = values.back();
    values.pop_back();
    assert(a.isBool() && b.isBool());
    switch (that->op) {
        case BoolExpr::OP_AND:
            values.push_back(DValue(a && b));
            break;
        case BoolExpr::OP_OR:
            values.push_back(DValue(a || b));
            break;
    }
}
