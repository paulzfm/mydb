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

using std::pair;
using std::vector;
using std::map;
using std::unordered_map;

typedef pair<Table*, RecordManager*> Container;

class QueryManager {
private:
	SystemManager *sysmgr;
	// <dbid, table name> -> <Table, RecordManager>
	std::map<pair<int, string>, Container> tables;

    std::function<bool(const Record&)> getFilter(BoolExpr* expr);

public:
	QueryManager(SystemManager *sysmgr_);
	~QueryManager();
	Container getContainer(const string& name);

	// tableName, [<attrName, data>]
	bool Insert(const string& table, unordered_map<string, char*>& data, string& msg);

	// tableName, condition expr
	bool Delete(const string& table, BoolExpr* where, string& msg);

	// tableName, <attrName -> expr>, condition expr
	bool Update(const string& table, unordered_map<string, Expr*>& data, BoolExpr* where, string& msg);

	// [<tableName, attrName>], condition expr, group by
	bool Select(vector<pair<string, string>>& attrs, BoolExpr* where, string groupBy, string& msg);

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
};

extern Container NullContainer;

#endif // QUERY_MANAGER_H_
