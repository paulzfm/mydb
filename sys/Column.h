#ifndef COLUMN_H_
#define COLUMN_H_

#include <stdint.h>
#include <string>
#include <vector>

struct ColumnDef {
	static const int bytes = 28;

	int cid;	// internal unique id starts from 0
	char name[16];
	int type;	// data type, use type id from DType
	int size;	// column size
	int offset; // offset in record
};

struct ColumnConstraint {
	static const int bytes = 16;

	int cid;
	/*
		0 - primary key
		1 - not null
		2 - unique
		3 - foreign key
		4 - default
		5 - check
	*/
	int type;
	int64_t data;
};

class Column {
	public:
		ColumnDef def;
		std::vector<ColumnConstraint> constraints;

		Column();
		Column(const ColumnDef &def);
		bool addConstraint(ColumnConstraint c);
		bool check(const char* data) const;
};

extern Column NullColumn;

#endif // COLUMN_H_
