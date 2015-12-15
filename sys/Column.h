#ifndef COLUMN_H_
#define COLUMN_H_

#include "../util/common.h"
#include <stdint.h>

class Column {
public:
	short cid;			// internal unique id starts from 0
	string name;	// column name
	char type;			// data type, use type id from DType
	short size;			// column size
	short offset;		// offset in record

	Column();
	Column(int cid, const string& name, char type, short size, short offset);

	rapidjson::Value serialize(rapidjson::Document& doc) const;
	void unserialize(const rapidjson::Value& value);
};

extern Column NullColumn;

#endif // COLUMN_H_
