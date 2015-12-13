#include "Expr.h"
#include <cassert>

Expr::Expr(const Operator& _op, const std::string& _column,
		const vector<Expr*>& _childs) : op(_op), column(_column), childs(_childs) {
	value.null = true;
}

void Expr::setValue(const Value& val) {
	assert(childs.size() == 0);
	this->val = val;
}

Value Expr::getValue() {
	if (value.null) {
		assert(op != NullOp);
		value = op.apply(childs);
	}
	return value;
}

std::function<bool(const Record&)> Expr::getFilter(const Table*) const {
}
