#include "QueryManager.h"
#include "Evaluator.h"

Container NullContainer = make_pair((Table*)NULL, (RecordManager*)NULL);

Container QueryManager::getContainer(const string& name) {
	pair<int, string> idx = std::make_pair(sysmgr->getCurrentDBId(), name);
	if (tables.find(idx) == tables.end()) {
		Database& db = sysmgr->getCurrentDB();
		int tid = db.getTableByName(name);
		if (tid == -1) {
			cmsg << "[ERROR] Table " + name + " not found!" << endl;
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

std::function<bool(const Record&)> QueryManager::getFilter(BoolExpr* expr) {
    return [this, expr] (const Record& rec) {
        Evaluator eval(this, rec.data);
        expr->accept(&eval);
        return eval.getValues().back().getBool();
    };
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

bool QueryManager::Insert(const string& table, unordered_map<string, char*>& data, string& msg) {
    cmsg.str("");
	Container rm = getContainer(table);
	if (rm == NullContainer) return false;
	
	char* buf = new char[rm.second->width()];
	memset(buf, 0, sizeof(char) * rm.second->width());

    // intialize null column to all 1
    Column& col = rm.first->getColumn(rm.first->getColumnById(0));
    memset(buf + col.offset, -1, col.size);
    char* null = buf + col.offset;

    // fill default values
    for (auto& con : rm.first->constraints) {
        if (con.type != Constraint::DEFAULT) continue;
        rm.first->setColumnValue(buf, con.cid, con.data);
        null[con.cid / 8] &= ~(1 << (con.cid % 8));
    }

	// fill data
	for (auto& col : rm.first->columns) {
		// TODO: check
		auto iter = data.find(col.name);
		if (iter != data.end()) {
			memcpy(buf + col.offset, iter->second, col.size);
            null[col.cid / 8] &= ~(1 << (col.cid % 8));
        }
	}

	rm.second->insert(buf);

	delete [] buf;
    msg = cmsg.str();
}

bool QueryManager::Delete(const string& table, BoolExpr* where, string& msg) {
    cmsg.str("");
	Container rm = getContainer(table);
	if (rm == NullContainer) return false;

	// query then remove
	//std::function<bool(const Record&)> filter = expr.getFilter(rm.first);
    auto filter = getFilter(where);
	vector<Record> results;
	rm.second->query(filter, results);

	for (auto& rec : results)
		rm.second->remove(rec.page, rec.offset);
    msg = cmsg.str();
}

bool QueryManager::Update(const string& table,
		unordered_map<string, Expr*>& data, BoolExpr* expr, string& msg) {
    cmsg.str("");
	Container rm = getContainer(table);
	if (rm == NullContainer) return false;

	// query then update
	//std::function<bool(const Record&)> filter = expr.getFilter(rm.first);
    auto filter = getFilter(expr);
	vector<Record> results;
	rm.second->query(filter, results);

	// TODO: update

	for (auto& rec : results)
		rm.second->replace(rec.page, rec.offset, rec.data);
    msg = cmsg.str();
}

bool QueryManager::Select(vector<pair<string, string>>& attrs,
        BoolExpr* expr, string groupBy, string& msg) {
    cmsg.str("");
	// query
	//std::function<bool(const Record&)> filter = expr.getFilter(rm.first);
    auto filter = getFilter(expr);
	vector<Record> results;
	//rm.second->query(filter, results);

    msg = cmsg.str();
}

bool QueryManager::UseDatabase(const string& name, string& msg) {
    cmsg.str("");
	sysmgr->useDatabase(name);
    msg = cmsg.str();
}

bool QueryManager::ShowDatabase(string& msg) {
    cmsg.str("");
	sysmgr->showDatabases();
    msg = cmsg.str();
}

bool QueryManager::CreateDatabase(const string& name, string& msg) {
    cmsg.str("");
	sysmgr->createDatabase(name);
    msg = cmsg.str();
}

bool QueryManager::DropDatabase(const string& name, string& msg) {
    cmsg.str("");
	sysmgr->dropDatabase(name);
    msg = cmsg.str();
}

bool QueryManager::ShowTables(string& msg) {
    cmsg.str("");
	sysmgr->showTables();
    msg = cmsg.str();
}

bool QueryManager::DescTable(const string& name, string& msg) {
    cmsg.str("");
	sysmgr->descTable(name);
    msg = cmsg.str();
}

bool QueryManager::CreateTable(const string& name, vector<Column>& cols,
    cmsg.str("");
		vector<Constraint>& cons, string& msg) {
    // TODO: nulls column
	Table& table = sysmgr->createTable(name, cols.size());
    vector<int> cids;
    cids.resize(cols.size());
    int i = 0;
	for (auto& col : cols) {
		cids[i++] = table.addColumn(col);
    }
	for (auto& con : cons) {
        con.cid = cids[con.cid];
		table.addConstraint(con);
    }
    msg = cmsg.str();
}

bool QueryManager::DropTable(const string& name, string& msg) {
    cmsg.str("");
	sysmgr->dropTable(name);
    msg = cmsg.str();
}
