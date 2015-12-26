#ifndef CONSTRAINT_H_
#define CONSTRAINT_H_

#include "../util/common.h"

class Constraint {
public:
    const static int NOT_NULL = 0;
    const static int UNIQUE = 1;
    const static int PRIMARY_KEY = 2;
    const static int FOREIGN_KEY = 3;
    const static int CHECK = 4;
    const static int DEFAULT = 5;
    const static int AUTO_INCREMENT = 6;

	short cid;		// reference to column id
	string name;
	char type;
	rapidjson::Value data;

	Constraint();
	Constraint(short cid, const string& name, char type,
		rapidjson::Value& data);

	rapidjson::Value serialize(rapidjson::Document& doc) const;
	void unserialize(rapidjson::Value value);
};

#endif
