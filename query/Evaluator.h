#ifndef EVALUATOR_H_
#define EVALUATOR_H_

#include "../util/common.h"
#include "../parser/Tree.h"
#include "../sys/Table.h"
#include "values.h"
#include "QueryManager.h"

class Evaluator : public Visitor {
private:
    vector<DValue> values;
    const unordered_map<string, DValue>& rec;
    string tb;

public:
    Evaluator(const unordered_map<string, DValue>& rec, string tb = "");
    vector<DValue>& getValues();

    bool visitTree(Tree *that);
    bool visitCol(Col *that);
    bool visitValue(Value *that);
    bool visitUnonExpr(UnonExpr *that);
    bool visitBinExpr(BinExpr *that);
    bool visitNullExpr(NullExpr *that);
    bool visitCompareExpr(CompareExpr *that);
    bool visitInExpr(InExpr *that);
    bool visitBetweenExpr(BetweenExpr *that);
    bool visitComplexExpr(ComplexExpr *that);
};

#endif
