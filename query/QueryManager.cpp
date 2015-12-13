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

void QueryManager::Insert(const string& table, unordered_map<string, char*> data) {
	Container& c = getContainer(table);
	if (rm == NULL) return;
	
	char* buf = new char[rm.second->width()];
	memset(buf, 0, sizeof(char) * rm.second->width());
	// fill data
	for (auto& col : rm.first->columns) {
		// TODO: fill default values
		// TODO: check
		auto iter = data.find(col.def.name);
		if (iter != data.end())
			memcpy(buf + col.def.offset, iter->second, col.def.size);
	}

	rm.second->insert(data);

	delete [] data;
}

void QueryManager::Delete(const string& table, Expr* expr) {
	Container& rm = getContainer(table);
	if (rm == NULL) return;

	// query then remove
	std::function<bool(const Record&)> filter = expr.getFilter(rm.first);
	vector<Record> results;
	rm.second->query(filter, results);

	for (auto& rec : results)
		rm.second->remove(rec);
}

void QueryManager::Update(const string& table, vector<pair<string, Expr*>>, Expr* expr) {
	Container& rm = getContainer(table);
	if (rm == NULL) return;

	// query then update
	std::function<bool(const Record&)> filter = expr.getFilter(rm.first);
	vector<Record> results;
	rm.second->query(filter, results);

	// TODO: update

	for (auto& rec : results)
		rm.second->replace(rec);
}

vector<Record>* QueryManager::Select(const string& table, vector<string> attrs, Expr* expr) {
	Container& rm = getContainer(table);
	if (rm == NULL) return;

	// query
	std::function<bool(const Record&)> filter = expr.getFilter(rm.first);
	vector<Record>* results = new vector<Record>();
	rm.second->query(filter, *results);
	return results;
}

void QueryManager::UseDatabase(const string& name) {
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
