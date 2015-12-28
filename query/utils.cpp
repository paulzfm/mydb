#include "utils.h"

std::function<bool(const Record&)> getFilter(Table* table, BoolExpr* expr) {
    return [expr, table] (const Record& rec) {
        unordered_map<string, DValue> values;
        string prefix = table->name + ".";
        DValue null = table->getColumnValue(rec.data, 0), val;
        for (const auto& col : table->columns) {
            if (col.cid == 0) continue; // NULL
            if ((null.data[col.cid / 8] & (1 << (col.cid % 8))) > 0)
                val = DValue();
            else
                val = table->getColumnValue(rec.data, col.cid);
            values[prefix + col.name] = val;
        }
        Evaluator eval(values, table->name);
        expr->accept(&eval);
        return eval.getValues().back().getBool();
    };
}

std::function<bool(const unordered_map<string, DValue>&)> getJoinFilter(BoolExpr* expr) {
    return [expr] (const unordered_map<string, DValue>& values) {
        Evaluator eval(values);
        expr->accept(&eval);
        return eval.getValues().back().getBool();
    };
}

Value* convValue(const DValue& val) {
    if (val.isBool()) {
        if (val.getBool()) return new Value(Value::VALUE_TRUE);
        else return new Value(Value::VALUE_FALSE);
    }
    if (val.isInt()) {
        return new Value(Value::VALUE_INT, val.printToString().c_str());
    }
    if (val.isReal()) {
        return new Value(Value::VALUE_REAL, val.printToString().c_str());
    }
    if (val.isString()) {
        return new Value(Value::VALUE_REAL, val.getString().c_str());
    }
    return new Value(Value::VALUE_NULL);
}

BoolExpr* getBoolExpr(const string& tb, const string& col, DValue val) {
    Col *column = new Col(tb.c_str(), col.c_str());
    Value *value = convValue(val);
    BoolExpr *expr = new CompareExpr(column, value, BoolExpr::OP_EQ);
    return expr;
}
