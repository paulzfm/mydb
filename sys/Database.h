#ifndef DATABASE_H_
#define DATABASE_H_

#include <vector>
#include <string>
#include "Table.h"

#define TABLELIST_FILE	"tablelist.dat"

class Database {
	public:
		std::string name;
		std::vector<Table> tables;

		Database();
		Database(const std::string& name);
		~Database();

		// returns -1 when not found
		int getTableByName(const std::string& name) const;

		void showTables() const;
		void descTable(const std::string& name) const;
		Table& createTable(const std::string& name);
		void dropTable(const std::string& name);
};

extern Database NullDatabase;

#endif // DATABASE_H_
