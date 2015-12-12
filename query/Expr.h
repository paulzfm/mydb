#ifndef EXPR_H_
#define EXPR_H_

#include "../record/RecordManager.h"
#include <functional>

class Expr {
};

class ValueExpr : Expr {
public:
	char* getValue(const Table*) const;
};

class CondExpr : Expr {
public:
	std::function<bool(const Record&)> getFilter(const Table*) const;
};

#endif // EXPR_H_
