#include "QueryManager.h"

Container NullContainer = make_pair((Table*)NULL, (RecordManager*)NULL);

Container QueryManager::getContainer(const string& name) {
	pair<int, string> idx = std::make_pair(sysmgr->getCurrentDBId(), name);
	if (tables.find(idx) == tables.end()) {
		Database& db = sysmgr->getCurrentDB();
		int tid = db.getTableByName(name);
		if (tid == -1) {
			std::cerr << "[ERROR] Table " + name + " not found!" << std::endl;
			return NullContainer;
		}

		string path = Configuration::conf()->basepath + db.name;
		path += '/' + name + ".dat";
		RecordManager* rm = new RecordManager(path);
		//return tables[idx] = std::make_pair(new Table(db.getTable(tid)), rm);
		return tables[idx] = std::make_pair((Table*)NULL, rm);

	} else {
		return tables[idx];
	}
}

QueryManager::QueryManager(SystemManager* sysmgr_) : sysmgr(sysmgr_) {
}

QueryManager::~QueryManager() {
	for (auto tb : tables) {
		delete tb.second.first;
		delete tb.second.second;
	}
	tables.clear();
}

void QueryManager::Insert(const string& table, unordered_map<string, char*>& data) {
	Container rm = getContainer(table);
	if (rm == NullContainer) return;
	
	char* buf = new char[rm.second->width()];
	memset(buf, 0, sizeof(char) * rm.second->width());
	// fill data
	for (auto& col : rm.first->columns) {
		// TODO: fill default values
		// TODO: check
		auto iter = data.find(col.name);
		if (iter != data.end())
			memcpy(buf + col.offset, iter->second, col.size);
	}

	rm.second->insert(buf);

	delete [] buf;
}

void QueryManager::Delete(const string& table, BoolExpr* expr) {
	Container rm = getContainer(table);
	if (rm == NullContainer) return;

	// query then remove
	//std::function<bool(const Record&)> filter = expr.getFilter(rm.first);
	vector<Record> results;
	//rm.second->query(filter, results);

	for (auto& rec : results)
		rm.second->remove(rec.page, rec.offset);
}

void QueryManager::Update(const string& table,
		unordered_map<string, Expr*>& data, BoolExpr* expr) {
	Container rm = getContainer(table);
	if (rm == NullContainer) return;

	// query then update
	//std::function<bool(const Record&)> filter = expr.getFilter(rm.first);
	vector<Record> results;
	//rm.second->query(filter, results);

	// TODO: update

	for (auto& rec : results)
		rm.second->replace(rec.page, rec.offset, rec.data);
}

vector<Record>* QueryManager::Select(const string& table,
		vector<string>& attrs, BoolExpr* expr, string groupBy) {
	Container rm = getContainer(table);
	if (rm == NullContainer) return NULL;

	// query
	//std::function<bool(const Record&)> filter = expr.getFilter(rm.first);
	vector<Record>* results = new vector<Record>();
	//rm.second->query(filter, *results);
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

void QueryManager::CreateTable(const string& name, vector<Column>& cols,
		vector<Constraint>& cons) {
	Table& table = sysmgr->createTable(name, cols.size());
	for (auto& col : cols)
		table.addColumn(col);
	for (auto& con : cons)
		table.addConstraint(con);
}

void QueryManager::DropTable(const string& name) {
	sysmgr->dropTable(name);
}
