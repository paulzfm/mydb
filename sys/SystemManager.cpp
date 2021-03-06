#include "SystemManager.h"
#include "../util/print.h"

#include <unistd.h>
#include <sys/stat.h>
#include <cstdlib>
#include <iostream>
#include <fstream>

SystemManager::SystemManager() {
	chdir(Configuration::conf()->basepath.c_str());
	std::ifstream fin(DBLIST_FILE);
	dbid = -1;
	if ( ! fin.is_open() ) {
		cmsg << "[WARNING] database list file " << DBLIST_FILE << " not found, empty list created." << endl;
		std::ofstream fout(DBLIST_FILE);
		fout << 0 << endl;
		fout.close();
		return;
	}

	int n;
	std::string name;
	fin >> n;
	for (int i = 0; i < n; ++i) {
		fin >> name;
        Database db;
        db.open(name);
		dbs.push_back(std::move(db));
	}
	fin.close();
}

SystemManager::~SystemManager() {
	std::ofstream fout(DBLIST_FILE);
	if ( ! fout.is_open() ) {
		cmsg << "[ERROR] cannot save database list file" << endl;
		return;
	}

	fout << dbs.size() << endl;
	for (auto& db : dbs) {
        db.close();
		fout << db.name << endl;
    }
}

int SystemManager::getDBIdByName(const std::string& name) const {
	int dbid = -1;
	for (unsigned i = 0; i < dbs.size(); ++i)
		if (dbs[i].name == name) {
			dbid = i;
			break;
		}
	if (dbid == -1) cmsg << "[ERROR] database " << name << " not found!" << endl;
	return dbid;
}

int SystemManager::getCurrentDBId() const {
	return dbid;
}

Database& SystemManager::getDB(int index) {
	assert(index >= 0 && index < dbs.size());
	return dbs[index];
}

Database& SystemManager::getCurrentDB() {
	if (dbid >= 0 && dbid < dbs.size()) return dbs[dbid];
	else return NullDatabase;
}

Database& SystemManager::useDatabase(const std::string& name) {
	for (unsigned i = 0; i < dbs.size(); ++i)
		if (dbs[i].name == name) {
			dbid = i;
            cmsg << "Current database changed to " << name << endl;
			return dbs[i];
		}
    cmsg << "[ERROR] database " << name << " not exists." << endl;
	return NullDatabase;
}

void SystemManager::showDatabases() const {
    if (dbs.size() == 0) {
        cmsg << "No databases in set.\n";
    } else {
        std::vector<std::string> heads;
        heads.push_back("Database");
        std::vector< std::vector<std::string> > rows;
        for (auto& db : dbs) {
            std::vector<std::string> row;
            row.push_back(db.name);
            rows.push_back(row);
        }

        cmsg << tableToString(heads, rows);
        cmsg << dbs.size() << " rows in set.\n";
    }
}

Database& SystemManager::createDatabase(const std::string& name) {
	for (auto &db : dbs)
		if (db.name == name) {
			cmsg << "[ERROR] database " << name << " already exists, skipping..." << endl;
			return NullDatabase;
		}

	mkdir(name.c_str(), 0755);
	std::ofstream fout((name + "/" + TABLELIST_FILE).c_str());
	fout << "{\"table_num\":0, \"name\":\"" + name + "\"}" << endl;
	fout.close();

	Database db(name);
	dbs.push_back(std::move(db));

    cmsg << "Database " << name << " created." << endl;

	return dbs.back();
}

void SystemManager::dropDatabase(const std::string& name) {
	int dbid = getDBIdByName(name);
	if (dbid == -1) {
		cmsg << "[ERROR] database " << name << " not exists, skipping..." << endl;
		return;
	}

	dbs.erase(dbs.begin() + dbid);

	char cmd[256];
	sprintf(cmd, "rm -rf %s", name.c_str());
	system(cmd);

    cmsg << "Database " << name << " has been dropped." << endl;
}

void SystemManager::showTables() const {
	if (dbid == -1) {
		cmsg << "[ERROR] no database selected." << endl;
		return;
	}
	dbs[dbid].showTables();
}

void SystemManager::descTable(const std::string& name) const {
	if (dbid == -1) {
		cmsg << "[ERROR] no database selected." << endl;
		return;
	}
	dbs[dbid].descTable(name);
}

Table& SystemManager::createTable(const std::string& name, int count) {
	if (dbid == -1) {
		cmsg << "[ERROR] no database selected." << endl;
		return NullTable;
	}
    cmsg << "Table " << name << " created." << endl;
	return dbs[dbid].createTable(name, count);
}

void SystemManager::dropTable(const std::string& name) {
	if (dbid == -1) {
		cmsg << "[ERROR] no database selected." << endl;
		return;
	}
	dbs[dbid].dropTable(name);
    cmsg << "Table " << name << " has been dropped." << endl;
}
