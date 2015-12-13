#ifndef OPERATOR_H_
#define OPERATOR_H_

#include <vector>
#include "Value.h"

class Operator {
public:
	virtual Value apply(const std::vector<Value>& values) const;
};

extern Operator NullOp;

class AddOperator {
public:
	virtual Value apply(const std::vector<Value>& values) const;
};

class MinusOperator {
public:
	virtual Value apply(const std::vector<Value>& values) const;
};

class MultiplyOperator {
public:
	virtual Value apply(const std::vector<Value>& values) const;
};

class DivideOperator {
public:
	virtual Value apply(const std::vector<Value>& values) const;
};

class ModOperator {
public:
	virtual Value apply(const std::vector<Value>& values) const;
};

class IsNullOperator {
public:
	virtual Value apply(const std::vector<Value>& values) const;
};

class EqualOperator {
public:
	virtual Value apply(const std::vector<Value>& values) const;
};

class InequalOperator {
public:
	virtual Value apply(const std::vector<Value>& values) const;
};

class GreaterOperator {
public:
	virtual Value apply(const std::vector<Value>& values) const;
};

class LessOperator {
public:
	virtual Value apply(const std::vector<Value>& values) const;
};

class GreaterOrEqualOperator {
public:
	virtual Value apply(const std::vector<Value>& values) const;
};

class LessOrEqualOperator {
public:
	virtual Value apply(const std::vector<Value>& values) const;
};

class AndOperator {
public:
	virtual Value apply(const std::vector<Value>& values) const;
};

class OrOperator {
public:
	virtual Value apply(const std::vector<Value>& values) const;
};

class NotOperator {
public:
	virtual Value apply(const std::vector<Value>& values) const;
};

class LikeOperator {
public:
	virtual Value apply(const std::vector<Value>& values) const;
};

#endif
