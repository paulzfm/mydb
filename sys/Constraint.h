#ifndef CONSTRAINT_H_
#define CONSTRAINT_H_

#include "../util/common.h"

class Constraint {
private:
public:
    const static int NOT_NULL = 0; // data is empty
    const static int UNIQUE = 1; // data is empty
    const static int PRIMARY_KEY = 2; // data is empty
    const static int FOREIGN_KEY = 3; // data shows the referenced column:
                                      // { "tb": table, "col": column }
    const static int CHECK = 4; // data shows the BoolExpr
                                // when used, calling
                                //     Tree* toTreeNode(const rapidjson::Value& val);
                                // in parser/Tree.h to unserialze to type BoolExpr
    const static int DEFAULT = 5; // data shows the default Value
                                  // when used, calling
                                  //     Tree* toTreeNode(const rapidjson::Value& val);
                                  // in parser/Tree.h to unserialze to type Value
    const static int AUTO_INCREMENT = 6; // data is empty

	short cid;		// reference to column id
	string name;    // column name
	char type;      // which type
	rapidjson::Value data; // see the comments after each type

	Constraint();
    Constraint(const Constraint& con);
	Constraint(short cid, const string& name, char type,
		const rapidjson::Value& data);

    void setData(const rapidjson::Value& data);
    rapidjson::Value getData() const;

	rapidjson::Value serialize(rapidjson::Document& doc) const;
	void unserialize(rapidjson::Value value);
};

#endif
