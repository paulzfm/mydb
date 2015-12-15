#ifndef DATABASE_H_
#define DATABASE_H_

#include "../util/common.h"
#include "Table.h"

#define TABLELIST_FILE	"tablelist.dat"

class Database {
private:
	int maxtid;

public:
	string name;
	vector<Table> tables;

	Database();
	Database(const string& name);

	bool open(const string& name);
	bool close() const;

	// returns NullTable when not found
	int getTableByName(const string& name) const;
	Table& getTable(int index);

	void showTables() const;
	void descTable(const string& name) const;
	Table& createTable(const string& name, int count);
	void dropTable(const string& name);
};

extern Database NullDatabase;

#endif // DATABASE_H_
