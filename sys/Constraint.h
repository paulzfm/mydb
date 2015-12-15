#ifndef CONSTRAINT_H_
#define CONSTRAINT_H_

#include "../util/common.h"

class Constraint {
public:
	short cid;		// reference to column id
	string name;
	char type;		// 0:NN, 1:UQ, 2:PK, 3:FK, 4:CHK, 5:DEFAULT
	Value data;

	Constraint();
	Constraint(short cid, const string& name, char type,
		Value& data);

	Value serialize(Document& doc) const;
	void unserialize(Value value);
};

#endif
