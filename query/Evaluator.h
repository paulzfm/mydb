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
    const char* rec;
    QueryManager* qm;
    
public:
    Evaluator(QueryManager* qm, const char* rec);
    vector<DValue>& getValues();

    void visitCol(Col *that);
    void visitValue(Value *that);
    void visitUnonExpr(UnonExpr *that);
    void visitBinExpr(BinExpr *that);
    void visitNullExpr(NullExpr *that);
    void visitCompareExpr(CompareExpr *that);
    void visitInExpr(InExpr *that);
    void visitBetweenExpr(BetweenExpr *that);
    void visitComplexExpr(ComplexExpr *that);
};

#endif 
