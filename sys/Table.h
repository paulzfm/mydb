#ifndef TABLE_H_
#define TABLE_H_

#include <string>
#include <vector>
#include <fstream>
#include "Column.h"

struct TableDef {
	static const int bytes = 18;

	char name[16];
	char column_num;
	char constraint_num;
};

class Table {
	private:
		int maxcid;
		int pk;		// cid

	public:
		std::string name;	// name of table
		std::vector<Column> columns;

		Table();
		Table(const std::string& name);

		Column& getColumnById(int cid);
		Column& getColumnByIndex(int index);
		Column& getColumnByName(std::string name);

		void addColumn(Column col);
		void removeColumn(int cid);

		bool open(std::ifstream& fin, const TableDef& def);
		bool close(std::ofstream& fout) const;
		void desc() const;
};

extern Table NullTable;

#endif // TABLE_H_