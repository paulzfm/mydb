#ifndef OPERATOR_H_
#define OPERATOR_H_

#include <vector>
#include "Value.h"

class Expr;

class Operator {
public:
	virtual Value apply(std::vector<Expr*>& childs) const = 0;
};

extern Operator NullOp;

#endif
