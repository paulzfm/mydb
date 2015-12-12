#include "Table.h"
#include "config.h"
#include <cstring>
#include <iostream>

Table NullTable = Table();

Table::Table() {
	this->pk = -1;
	this->length = 0;
}

Table::Table(const std::string& name, int count) {
	this->name = name;
	this->pk = -1;
	this->length = (count - 1) / 8 + 1;
}

Column& Table::getColumnById(int cid) {
	for (auto &col : columns)
		if (col.def.cid == cid) return col;
	return NullColumn;
}

Column& Table::getColumnByIndex(int index) {
	if ((int)columns.size() <= index) return NullColumn;
	return columns[index];
}

Column& Table::getColumnByName(std::string name) {
	for (auto &col : columns)
		if (name == std::string(col.def.name)) return col;
	return NullColumn;
}

void Table::addColumn(Column col) {
	// check if column name is unique
	for (auto& c : columns)
		if (c.def.name == col.def.name) {
			std::cerr << "[ERROR] Column " << name << " already exist." << std::endl;
			return;
		}

	col.def.cid = ++maxcid;
	col.def.offset = this->length;
	this->length += col.def.size;
	for (auto& c : col.constraints)
		c.cid = maxcid;
	this->columns.push_back(std::move(col));
}

// TODO: rearrange data store in this table
void Table::removeColumn(int cid) {
	for (unsigned i = 0; i < this->columns.size(); ++i)
		if (columns[i].def.cid == cid) {
			columns.erase(columns.begin() + i);
			break;
		}
}

bool Table::open(std::ifstream& fin, const TableDef& def) {
	// read columns
	maxcid = 0;
	ColumnDef col;
	for (int i = 0; i < def.column_num; ++i) {
		fin.read((char*) &col, ColumnDef::bytes);
		this->columns.push_back(Column(col));
		if (col.cid > maxcid) maxcid = col.cid;
	}

	// read constraints
	ColumnConstraint cc;
	for (int i = 0; i < def.constraint_num; ++i) {
		fin.read((char*) &cc, ColumnConstraint::bytes);
		Column &col = getColumnById(cc.cid);

		if (cc.type == 0) {
			// TODO: error handling
			if (pk != -1) {
				std::cerr << "[ERROR] multiple primary key presented in table " << this->name << std::endl;
				return false;
			}
			pk = col.def.cid;
		}

		if (col.def.cid < 0) {
			std::cerr << "[ERROR] Column " << cc.cid << " not found, skipping..." << std::endl;
			continue;
		}
		col.constraints.push_back(cc);
	}

	if (pk == -1) return false;
	return true;
}

bool Table::close(std::ofstream& fout) const {
	// write column
	for (auto &col : columns)
		fout.write((char*) &col.def, ColumnDef::bytes);

	// write constraints
	for (auto &col : columns)
		for (auto &cons : col.constraints)
			fout.write((char*) &cons, ColumnConstraint::bytes);

	return true;
}

void Table::desc() const {
	std::cout << "------ Columns of Table " << name << " ------" << std::endl;
	for (auto& col : columns) {
		std::cout << "| " << col.def.name << " of type " << col.def.type << "\t";
		for (auto& cons : col.constraints) {
			switch (cons.type) {
				case 0:
					std::cout << " Primary Key\t";
					break;
				case 1:
					std::cout << " Not Null\t";
					break;
				case 2:
					std::cout << " Unique\t";
					break;
				case 3:
					std::cout << " Foreign Key\t";
					break;
				case 4:
					std::cout << " Default\t";
					break;
				case 5:
					std::cout << " Check\t";
					break;
			}
		}
		std::cout << std::endl;
	}
}
