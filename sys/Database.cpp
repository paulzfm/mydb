#include "Database.h"
#include "unistd.h"
#include <cstring>
#include <fstream>
#include <iostream>

Database NullDatabase = Database();

int Database::getTableByName(const std::string& name) const {
	for (unsigned i = 0; i < tables.size(); ++i)
		if (tables[i].name == name)
			return i;
	return -1;
}

Database::Database() {
}

Database::Database(const std::string& name) {
	this->name = name;

	std::ifstream fin((name + "/" + TABLELIST_FILE).c_str());
	if (fin.is_open()) {
		TableDef def;
		for (;;) {
			fin.read((char*)&def, TableDef::bytes);
			if (fin.gcount() != TableDef::bytes) break;
			Table table(def.name);
			table.open(fin, def);
			tables.push_back(std::move(table));
		}

		fin.close();
	} else {
		std::cerr << "[WARNING] table list of database " << name << " file not found, empty list created." << std::endl;
	}
}

Database::~Database() {
	std::ofstream fout((name + "/" + TABLELIST_FILE).c_str());
	for (auto& table : tables) {
		TableDef def;
		strcpy(def.name, table.name.c_str());
		def.column_num = table.columns.size();
		def.constraint_num = 0;
		for (auto& col : table.columns)
			def.constraint_num += col.constraints.size();
		fout.write((char*) &def, TableDef::bytes);

		table.close(fout);
	}
}

void Database::showTables() const {
	std::cout << "-- LIST OF TABLES" << std::endl;
	for (auto& table : tables) {
		std::cout << ">> " << table.name << std::endl;
	}
}

void Database::descTable(const std::string& name) const {
	int tid = getTableByName(name);
	if (tid == -1) {
		std::cerr << "[ERROR] Table " << name << " not exists." << std::endl;
		return;
	}
	
	tables[tid].desc();
}

Table& Database::createTable(const std::string& name) {
	// check if name is unique
	int tid = getTableByName(name);
	if (tid != -1) {
		std::cerr << "[ERROR] Table " << name << " already exist." << std::endl;
		return NullTable;
	}

	Table table(name);
	tables.push_back(std::move(table));
	return tables.back();
}

void Database::dropTable(const std::string& name) {
	int tid = getTableByName(name);
	if (tid == -1) {
		std::cerr<< "[ERROR] Table " << name << " not exists." << std::endl;
		return;
	}

	tables.erase(tables.begin() + tid);
}
