#ifndef QUERY_MANAGER_H_
#define QUERY_MANAGER_H_

#include <vector>
#include <utility>
#include <unordered_map>
#include "../record/RecordManager.h"
#include "../sys/SystemManager.h"
#include "../sys/Table.h"
#include "../sys/Column.h"

using std::string;
using std::pair;
using std::vector;

typedef pair<Table*, RecordManager*> Container;

class QueryManager {
private:
	SystemManager *sysmgr;
	// <dbid, table name> -> <Table, RecordManager>
	std::unordered_map<pair<int, string>, Container> tables;

	Container getContainer(const string& name);

public:
	QueryManager(SystemManager *sysmgr_);
	~QueryManager();

	// tableName, [<attrName, data>]
	void Insert(const string& table, vector<pair<string, char*>> data);
	// tableName, condition expr
	void Delete(const string& table, Expr* expr);
	// tableName, [<attrName, expr>], condition expr
	void Update(const string& table, vector<pair<string, Expr*>>, Expr* expr);
	// tableName, [attrName], condition expr
	vector<Record>* Select(const string& table, vector<string> attrs, Expr* expr);
	// tableName
	// * caller should build up all Columns with constraints
	void CreateTable(const string& name, vector<Column> cols);

	// wrapper
	void UseDatabase(const string& name);
	void ShowDatabase();
	void CreateDatabase(const string& name);
	void DropDatabase(const string& name);

	void ShowTables();
	void DescTable(const string& name);
	void DropTable(const string& name);
};

#endif // QUERY_MANAGER_H_
