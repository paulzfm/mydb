#ifndef TABLE_H_
#define TABLE_H_

#include "../util/common.h"
#include "Column.h"
#include "Constraint.h"

class Table {
	private:
		int tid;
		int maxcid;
		int width;

	public:
		std::string name;	// name of table
		std::vector<Column> columns;
		std::vector<Constraint> constraints;

		Table();
		Table(int tid, const std::string& name);

		int getTid() const;
		int getWidth() const;
		string getName() const;

		int getColumnById(int cid) const ;
		int getColumnByName(std::string name) const;

		Column& getColumn(int index);

		int addColumn(Column& col);
		bool removeColumn(int cid);
		bool addConstraint(Constraint& c);

		bool open(std::ifstream& fin);
		bool close(std::ofstream& fout) const;
		void desc() const;

        // util functions
		bool checkConstraints(const char* rec) const;
        bool setColumnValue(char* rec, short cid, const rapidjson::Value& val) const;
        char* getColumnValue(char* rec, short cid) const;
};

extern Table NullTable;

#endif // TABLE_H_
