#ifndef EXPR_H_
#define EXPR_H_

#include "../record/RecordManager.h"
#include "Operator.h"
#include "Value.h"
#include <functional>

using std::vector;

class Expr {
private:
	Operator op;
	std::string column;
	vector<Expr*> childs;
	Value value;

public:
	Expr(const Operator& op, const std::string& column, const vector<Expr*>& childs);
	void setValue(const Value& val);
	Value getValue();
	std::function<bool(const Record&)> getFilter(const Table*) const;
};

#endif // EXPR_H_
