#include "QueryManager.h"
#include <iostream>

Container& QueryManager::getContainer(const string& name) {
	pair<int, string> idx = std::make_pair(sysmgr->dbid, name);
	if (tables.find(idx) == tables.end()) {
		Database& db = sysmgr->dbs[sysmgr->dbid];
		int tid = db->getTableByName(name);
		if (tid == -1) {
			std::cerr << "[ERROR] Table " + name + " not found!" << std::endl;
			return NULL;
		}

		string path = Configuration::conf()->basepath + db.name;
		path += '/' + name + ".dat";
		RecordManager* rm = new RecordManager(path);
		return tables[idx] = std::make_pair(new Table(db->tables[tid]), rm);

	} else {
		return tables[idx];
	}
}

QueryManager::QueryManager(SystemManager sysmgr_) : sysmgr(sysmgr_) {
}

QueryManager::~QueryManager() {
	for (auto tb : tables) {
		delete tb.first;
		delete tb.second;
	}
	tb.clear();
}

void QueryManager::Insert(const string& table, vector<pair<string, char*>> data) {
	Container& c = getContainer(table);
	if (rm == NULL) return;
	
	char* data = new char[rm.second->width()];
	memset(data, 0, sizeof(char) * rm.second->width());
	// TODO: fill data
	for (auto& col : c.first->columns) {
	}

	rm.second->insert(data);

	delete [] data;
}

void QueryManager::Delete(const string& table, CondExpr expr) {
	Container& rm = getContainer(table);
	if (rm == NULL) return;

	// query then remove
	std::function<bool(const Record&)> filter = expr.getFilter(rm.first);
	vector<Record> results;
	rm.second->query(filter, results);
}

void QueryManager::Update(const string& table, vector<pair<string, ValueExpr>>, CondExpr expr) {
	Container& rm = getContainer(table);
	if (rm == NULL) return;

	// query then update
	std::function<bool(const Record&)> filter = expr.getFilter(rm.first);
	vector<Record> results;
	rm.second->query(filter, results);
}

vector<int> QueryManager::Select(const string& table, vector<string> attrs, CondExpr expr) {
	Container& rm = getContainer(table);
	if (rm == NULL) return;

	// query
	std::function<bool(const Record&)> filter = expr.getFilter(rm.first);
	vector<Record> results;
	rm.second->query(filter, results);
}

void QueryManager::Use(const string& name) {
	sysmgr->useDatabase(name);
}

void QueryManager::ShowDatabase() {
	sysmgr->showDatabases();
}

void QueryManager::CreateDatabase(const string& name) {
	sysmgr->createDatabase(name);
}

void QueryManager::DropDatabase(const string& name) {
	sysmgr->dropDatabase(name);
}

void QueryManager::ShowTables() {
	sysmgr->showTables();
}

void QueryManager::DescTable(const string& name) {
	sysmgr->descTable(name);
}

void QueryManager::CreateTable(const string& name, vector<Column> cols) {
	Table& table = sysmgr->createTable(name);
	for (auto& col : cols)
		table.addColumn(col);
}

void QueryManager::DropTable(const string& name) {
	sysmgr->dropTable(name);
}
