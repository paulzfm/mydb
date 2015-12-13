#include "Operator.h"

Operator NullOp;

Value Operator::apply(const std::vector<Value>& values) const {
	return NullValue;
}

Value PlusOperator::apply(const std::vector<Value>& values) const {
	if (checkSize(values, 2) || checkType(values, DType::CHAR) || checkType(values, DType::STRING))
		return NullValue;
	
}

