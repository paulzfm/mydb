#include "Evaluator.h"

Evaluator::Evaluator(const unordered_map<string, DValue>& rec, string tb)
        : rec(rec), tb(tb) {
}

vector<DValue>& Evaluator::getValues() {
    return values;
}

bool Evaluator::visitCol(Col *that) {
    string name = that->tb + "." + that->col;
    if (that->tb == "") name = tb + name;
    auto iter = rec.find(name);
    if (iter == rec.end()) {
        values.push_back(DValue());

    } else {
        values.push_back(iter->second);
    }

    return true;
}

bool Evaluator::visitValue(Value *that) {
    switch (that->kind) {
        case Value::VALUE_INT:
            values.push_back(DValue((int64_t)atoll(that->val.c_str())));
            break;
        case Value::VALUE_REAL:
            values.push_back(DValue(atof(that->val.c_str())));
            break;
        case Value::VALUE_STRING:
            values.push_back(DValue(that->val));
            break;
        case Value::VALUE_NULL:
            values.push_back(DValue());
            break;
        case Value::VALUE_TRUE:
            values.push_back(DValue(true));
            break;
        case Value::VALUE_FALSE:
            values.push_back(DValue(false));
            break;
    }

    return true;
}

bool Evaluator::visitUnonExpr(UnonExpr *that) {
    that->expr->accept(this);
    DValue val = values.back();
    values.pop_back();
    if (that->op == Expr::OP_NEG) {
        assert(val.isNum());
        if (val.isInt()) val = DValue(-val.getInt());
        if (val.isReal()) val = DValue(-val.getReal());
    }
    values.push_back(val);

    return true;
}

bool Evaluator::visitBinExpr(BinExpr *that) {
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

    return true;
}

bool Evaluator::visitNullExpr(NullExpr *that) {
    // get null column value and visiting Column
    string name = that->name->tb + "." + that->name->col;
    if (that->name->tb == "") name = tb + name;
    auto iter = rec.find(name);
    if (iter == rec.end()) return false;
    bool res = iter->second.isNull();
    values.push_back(DValue(res != (that->op == BoolExpr::OP_IS_NOT_NULL)));

    return true;
}

bool Evaluator::visitCompareExpr(CompareExpr *that) {
    that->right->accept(this);
    that->left->accept(this);

    DValue val = values.back(), res;
    values.pop_back();
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

    return true;
}

bool Evaluator::visitInExpr(InExpr *that) {
    that->left->accept(this);
    bool res = false;
    for (Value* e : *that->right) {
        if ((DValue(e) == values.back()).getBool()) {
            res = true;
            break;
        }
    }
    values.pop_back();
    values.push_back(DValue(res != (that->op == BoolExpr::OP_NOT_IN)));

    return true;
}

bool Evaluator::visitBetweenExpr(BetweenExpr *that) {
    int low = atoll(that->rightL->val.c_str()),
        high = atoll(that->rightR->val.c_str());
    that->left->accept(this);
    DValue val = values.back();
    values.pop_back();
    assert(val.isNum());
    bool res;
    if (val.isInt())
        res = low <= val.getInt() && val.getInt() <= high;
    else
        res = low <= val.getReal() && val.getReal() <= high;
    values.push_back(DValue(res != (that->op == BoolExpr::OP_NOT_BETWEEN)));

    return true;
}

bool Evaluator::visitComplexExpr(ComplexExpr *that) {
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

    return true;
}
