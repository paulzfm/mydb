#include "SystemManager.h"
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
		std::cerr << "[WARNING] database list file " << DBLIST_FILE << " not found, empty list created." << std::endl;
		std::ofstream fout(DBLIST_FILE);
		fout << 0 << std::endl;
		fout.close();
		return;
	}

	int n;
	std::string name;
	fin >> n;
	for (int i = 0; i < n; ++i) {
		fin >> name;
		dbs.push_back(Database(name));
	}
	fin.close();
}

SystemManager::~SystemManager() {
	std::ofstream fout(DBLIST_FILE);
	if ( ! fout.is_open() ) {
		std::cerr << "[ERROR] cannot save database list file" << std::endl;
		return;
	}

	fout << dbs.size() << std::endl;
	for (auto& db : dbs)
		fout << db.name << std::endl;
}

int SystemManager::getDBIdByName(const std::string& name) const {
	int dbid = -1;
	for (unsigned i = 0; i < dbs.size(); ++i)
		if (dbs[i].name == name) {
			dbid = i;
			break;
		}
	if (dbid == -1) std::cerr << "[ERROR] database " << name << " not found!" << std::endl;
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
			return dbs[i];
		}
	return NullDatabase;
}

void SystemManager::showDatabases() const {
	cout << "-- LIST OF DATABASES, TOTAL " << dbs.size() << " --" << endl;
	for (auto& db : dbs) {
		std::cout << ">> " << db.name << std::endl;
	}
}

Database& SystemManager::createDatabase(const std::string& name) {
	for (auto &db : dbs)
		if (db.name == name) {
			std::cerr << "[ERROR] database " << name << " already exists, skipping..." << std::endl;
			return NullDatabase;
		}

	mkdir(name.c_str(), 0755);
	std::ofstream fout((name + "/" + TABLELIST_FILE).c_str());
	fout << "{\"table_num\":0, \"name\":\"" + name + "\"}" << endl;
	fout.close();

	Database db(name);
	dbs.push_back(std::move(db));

	return dbs.back();
}

void SystemManager::dropDatabase(const std::string& name) {
	int dbid = getDBIdByName(name);
	if (dbid == -1) {
		std::cerr << "[ERROR] database " << name << " not exists, skipping..." << std::endl;
		return;
	}

	dbs.erase(dbs.begin() + dbid);

	// TODO: error handling
	char cmd[256];
	sprintf(cmd, "rm -rf %s", name.c_str());
	system(cmd);
}

void SystemManager::showTables() const {
	if (dbid == -1) {
		std::cerr << "[ERROR] no database selected." << std::endl;
		return;
	}
	dbs[dbid].showTables();
}

void SystemManager::descTable(const std::string& name) const {
	if (dbid == -1) {
		std::cerr << "[ERROR] no database selected." << std::endl;
		return;
	}
	dbs[dbid].descTable(name);
}

Table& SystemManager::createTable(const std::string& name, int count) {
	if (dbid == -1) {
		std::cerr << "[ERROR] no database selected." << std::endl;
		return NullTable;
	}
	return dbs[dbid].createTable(name, count);
}

void SystemManager::dropTable(const std::string& name) {
	if (dbid == -1) {
		std::cerr << "[ERROR] no database selected." << std::endl;
		return;
	}
	dbs[dbid].dropTable(name);
}
