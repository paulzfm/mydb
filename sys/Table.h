#ifndef TABLE_H_
#define TABLE_H_

#include <string>
#include <vector>
#include "Column.h"

class Table {
	private:
		int maxcid;

	public:
		std::string name;	// name of table
		std::vector<Column> columns;

		Column& getColumnById(int cid);
		Column& getColumnByIndex(int index);
		Column& getColumnByName(std::string name);

		bool open(std::string name) const;
		bool close() const;
};

#endif // TABLE_H_
