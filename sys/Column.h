#ifndef COLUMN_H_
#define COLUMN_H_

#include <string>

struct ColumnDef {
	static const int bytes = 24;

	int cid;	// internal unique id starts from 0
	char name[16];
	int type;	// data type, use type id from DType
};

struct ColumnConstraint {
	static const int bytes = 16;

	int cid;
	int type;
	int64_t data;
};

class Column {
	public:
		ColumnDef def;
		std::vector<ColumnConstraint> constraints;

		Column();
		Column(ColumnDef &def);
		bool addConstraint(ColumnConstraint c);
		bool check(const char* data) const;
};

static const Column InvalidColumn = Column(ColumnDef{-1, "", 0});

#endif // COLUMN_H_
