#ifndef SYSTEM_MANAGER_H_
#define SYSTEM_MANAGER_H_

#include "../util/common.h"
#include "Database.h"
#include "config.h"

#define DBLIST_FILE		"dblist.dat"

class SystemManager {
	public:
		std::vector<Database> dbs;
		int dbid;	// currently opened db

		SystemManager();
		~SystemManager();

		int getDBIdByName(const string& name) const;
		Database& getDB(int index);

		Database& useDatabase(const string& name);
		void showDatabases() const;
		Database& createDatabase(const string& name);
		void dropDatabase(const string& name);

		void showTables() const;
		void descTable(const string& name) const;
		Table& createTable(const string& name, int count);
		void dropTable(const string& name);
};

#endif // SYSTEM_MANAGER_H_
