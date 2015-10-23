#include "Table.h"
#include "config.h"
#include <cstring>

Column& Table::getColumnById(int cid) {
	for (auto &col : columns)
		if (col.def.cid == cid) return col;
	return InvalidColumn;
}

Column& Table::getColumnByIndex(int index) {
	if (columns.size() <= index) return InvalidColumn;
	return columns[index];
}

Column& Table::getColumnByName(std::string name) {
	for (auto &col : columns)
		if (name == std::string(col.def.name)) return col;
	return InvalidColumn;
}

bool Table::open(std::string name) {
	std::string file = Configuration::conf()->basepath + name;
	std::ifstream fin;
	char buf[256];

	// read columns
	fin.open(file + ".col", std::fstream::in | std::fstream::binary);
	maxcid = 0;
	for (ColumnDef col; ;) {
		fin.read(buf, ColumnDef::bytes);
		if (fin.gcount() != ColumnDef::bytes) break;
		memcpy(&col, buf, ColumnDef::bytes);
		this->columns.push_back(Column(col));
		if (col.cid > maxcid) maxcid = col.cid;
	}
	fin.close();

	// read constraints
	fin.open(file + ".cons", std::fstream::in | std::fstream::binary);
	for (ColumnConstraint cc; ;) {
		fin.read(buf, ColumnConstraint::bytes);
		if (fin.gcount() != ColumnConstraint::bytes) break;
		memcpy(&cc, buf, ColumnConstraint::bytes);
		Column &col = getColumnById(cc.cid);
		if (col.def.cid < 0) {
			std::cerr << "[Error] Column " << cc.cid << " not found" << std::endl;
			continue;
		}
		col.constraints.push_back(cc);
	}
	fin.close();
}

bool Table::close() {
	std::string file = Configuration::conf()->basepath + name;
	std::ofstream colfile, consfile;
	char buf[256];

	// write columns and constraints
	colfile.open(file + ".col", std::fstream::out | std::fstream::binary);
	consfile.open(file + ".cons", std::fstream::out | std::fstream::binary);
	for (auto &col : columns) {
		// write column
		colfile.write((char*) &col.def, ColumnDef::bytes);

		for (auto &cons : col.constraints) {
			consfile.write((char*) &cons, ColumnConstraint::bytes);
		}
	}
	colfile.close();
	consfile.close();
}
