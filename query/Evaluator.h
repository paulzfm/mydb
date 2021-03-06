#ifndef EVALUATOR_H_
#define EVALUATOR_H_

#include "../util/common.h"
#include "../parser/Tree.h"
#include "../sys/Table.h"
#include "values.h"
#include "QueryManager.h"
#include <unordered_map>

using std::unordered_map;

class Evaluator : public Visitor {
private:
    vector<DValue> values;
    const unordered_map<string, DValue>& rec;
    string tb;

public:
    Evaluator(const unordered_map<string, DValue>& rec, string tb = "");
    vector<DValue>& getValues();

    bool visitCol(Col *that);
    bool visitValue(Value *that);
    bool visitUnonExpr(UnonExpr *that);
    bool visitBinExpr(BinExpr *that);
    bool visitBoolValue(BoolValue *that);
    bool visitNullExpr(NullExpr *that);
    bool visitCompareExpr(CompareExpr *that);
    bool visitInExpr(InExpr *that);
    bool visitBetweenExpr(BetweenExpr *that);
    bool visitComplexExpr(ComplexExpr *that);
};

#endif
