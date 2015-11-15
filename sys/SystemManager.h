#ifndef SYSTEM_MANAGER_H_
#define SYSTEM_MANAGER_H_

#include <vector>
#include <string>
#include "Database.h"
#include "config.h"

#define DBLIST_FILE		"dblist.dat"

class SystemManager {
	private:
		int getDBIdByName(const std::string& name) const;
	public:
		std::vector<Database> dbs;
		int dbid;	// currently opened db

		SystemManager();
		~SystemManager();

		Database& useDatabase(const std::string& name);
		void showDatabases() const;
		Database& createDatabase(const std::string& name);
		void dropDatabase(const std::string& name);

		void showTables() const;
		void descTable(const std::string& name) const;
		Table& createTable(const std::string& name);
		void dropTable(const std::string& name);
};

#endif // SYSTEM_MANAGER_H_
