#ifndef CONSTRAINT_H_
#define CONSTRAINT_H_

#include "../util/common.h"

class Constraint {
public:
	short cid;		// reference to column id
	string name;
	char type;		// 0:NN, 1:UQ, 2:PK, 3:FK, 4:CHK, 5:DEFAULT
	rapidjson::Value data;

	Constraint();
	Constraint(short cid, const string& name, char type,
		rapidjson::Value& data);

	rapidjson::Value serialize(rapidjson::Document& doc) const;
	void unserialize(rapidjson::Value value);
};

#endif
