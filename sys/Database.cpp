#include "Database.h"
#include "unistd.h"
#include <cstring>
#include <fstream>
#include <iostream>

Database NullDatabase = Database();

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
		std::cerr << "[ERROR] table list of database " << name << " file not found, empty list created." << std::endl;
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

Table& Database::createTable(const std::string& name) {
	Table table(name);
	tables.push_back(std::move(table));
	return tables.back();
}

void Database::dropTable(const std::string& name) {
}
