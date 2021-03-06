#ifndef QUERY_MANAGER_H_
#define QUERY_MANAGER_H_

#include "../util/common.h"
#include <map>
#include <unordered_map>
#include "../record/RecordManager.h"
#include "../sys/SystemManager.h"
#include "../sys/Table.h"
#include "../sys/Column.h"
#include "../parser/Tree.h"
#include "values.h"
#include "utils.h"

using std::pair;
using std::vector;
using std::map;
using std::unordered_map;

typedef pair<Table*, RecordManager*> Container;

class QueryManager {
private:
	SystemManager *sysmgr;
	// <dbid, table name> -> RecordManager
	std::map<pair<int, string>, RecordManager*> tables;

    //std::function<bool(const Record&)> getFilter(Table* table, BoolExpr* expr);
    //std::function<bool(const unordered_map<string, DValue>&)> getJoinFilter(BoolExpr* expr);

    bool join(const vector<string>& tables,
        const vector<vector<Record>>& rs,
        std::function<bool(const unordered_map<string, DValue>&)> filter,
        const Selectors& selectors,
        vector<unordered_map<string, DValue>>& results);

    bool group(const GroupBy& groupby,
        const Selectors& selectors,
        const vector<string>& tables,
        vector<unordered_map<string, DValue>>& input,
        vector<vector<unordered_map<string, DValue>>>& output);

    bool aggregate(const Selectors& selectors,
        const GroupBy& groupby,
        vector<vector<unordered_map<string, DValue>>>& input,
        vector<unordered_map<string, DValue>>& output);

    bool print(const Selectors& selectors,
        const vector<string>& tables,
        const vector<unordered_map<string, DValue>>& results);

public:
	QueryManager(SystemManager *sysmgr_);
	~QueryManager();

	Container getContainer(const string& name);

    // tableName, data
    bool Insert(const string& table, vector<Value*>& data, string& msg);
	// tableName, [<attrName, data>]
	bool Insert(const string& table, unordered_map<string, Value*>& data, string& msg);

	// tableName, condition expr
	bool Delete(const string& table, BoolExpr* where, string& msg);

	// tableName, <attrName -> expr>, condition expr
	bool Update(const string& table, unordered_map<string, Expr*>& data, BoolExpr* where, string& msg);

	// tables, [<tableName, attrName>], condition expr, group by
	bool Select(const vector<string>& tables, Selectors* selectors,
        BoolExpr* where, GroupBy* groupBy, string& msg);

	// tableName
	// * caller should build up all Columns with constraints
	bool CreateTable(const string& name, vector<Column>& cols, vector<Constraint>& cons, string& msg);

	// wrapper
	bool UseDatabase(const string& name, string& msg);
	bool ShowDatabase(string& msg);
	bool CreateDatabase(const string& name, string& msg);
	bool DropDatabase(const string& name, string& msg);

	bool ShowTables(string& msg);
	bool DescTable(const string& name, string& msg);
	bool DropTable(const string& name, string& msg);

    bool CreateIndex(const string& table, const string& col, string& msg);
    bool DropIndex(const string& table, const string& col, string& msg);
};

extern Container NullContainer;

#endif // QUERY_MANAGER_H_
