#include "QueryManager.h"
#include "Evaluator.h"
#include "../record/types.h"

Container NullContainer = make_pair((Table*)NULL, (RecordManager*)NULL);

bool setError(string& msg) {
    msg = cmsg.str();
    return false;
}

rapidjson::Value convValue(DValue& val) {
}

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

std::function<bool(const vector<Record>&)> QueryManager::getJoinFilter(BoolExpr* expr) {
    return [this, expr] (const vector<Record>&) {
        return true;
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
	if (rm == NullContainer) return setError(msg);
	
	char* buf = new char[rm.second->width()];
	memset(buf, 0, sizeof(char) * rm.second->width());

    // intialize null column to all 1
    Column& col = rm.first->getColumn(rm.first->getColumnById(0));
    memset(buf + col.offset, -1, col.size);
    char* null = buf + col.offset;

    // fill default values
    for (auto& con : rm.first->constraints) {
        if (con.type == Constraint::DEFAULT) {
            rm.first->setColumnValue(buf, con.cid, con.data);
            null[con.cid / 8] &= ~(1 << (con.cid % 8));
        }
        if (con.type == Constraint::AUTO_INCREMENT) {
            rm.first->setColumnValue(buf, con.cid, con.data);
        }
    }

	// fill data
	for (auto& col : rm.first->columns) {
		auto iter = data.find(col.name);
		if (iter != data.end()) {
			memcpy(buf + col.offset, iter->second, col.size);
            null[col.cid / 8] &= ~(1 << (col.cid % 8));
            data.erase(iter);
        }
	}
    
    // check NN, CHK constraints
    if ( ! rm.first->checkConstraints(buf) ) return setError(msg);
    // TODO: check UQ and FK

    for (auto& p : data) {
        cmsg << "[WARNING] Column " << p.first << "not found, ignored" << endl;
    }

	rm.second->insert(buf);

	delete [] buf;
    msg = cmsg.str();
    return true;
}

bool QueryManager::Delete(const string& table, BoolExpr* where, string& msg) {
    cmsg.str("");
	Container rm = getContainer(table);
	if (rm == NullContainer) return false;

	// query then remove
    auto filter = getFilter(where);
	vector<Record> results;
	rm.second->query(filter, results);

	for (auto& rec : results)
		rm.second->remove(rec.page, rec.offset);
    msg = cmsg.str();
    return true;
}

bool QueryManager::Update(const string& table,
		unordered_map<string, Expr*>& data, BoolExpr* expr, string& msg) {
    cmsg.str("");
	Container rm = getContainer(table);
	if (rm == NullContainer) return setError(msg);

	// query then update
    auto filter = getFilter(expr);
	vector<Record> results;
	rm.second->query(filter, results);

	// update
    for (auto& p : data) {
        int cid = rm.first->getColumnByName(p.first);
        if (cid == -1) return setError(msg);

        for (auto& rec : results) {
            Evaluator eval(this, rec.data);
            rm.first->setColumnValue(rec.data, cid, convValue(eval.getValues().back()));
        }
    }

    for (auto& rec : results) {
        // check NN, CHK constraints
        if ( ! rm.first->checkConstraints(rec.data) ) return setError(msg);
        // TODO: check UQ and FK
    }

	for (auto& rec : results)
		rm.second->replace(rec.page, rec.offset, rec.data);

    msg = cmsg.str();
    return true;
}

bool executeJoin(const vector<string>& tables,
        const vector<vector<Record>>& rs,
        std::function<bool(const vector<Record>&)> filter,
        const Selectors& selectors,
        vector<vector<DValue>>& results) {
    return true;
}

bool QueryManager::Select(const vector<string>& tables, const Selectors& selectors,
        BoolExpr* expr, GroupBy& groupBy, string& msg) {
    cmsg.str("");
    unordered_map<string, Container> rms;

    // get tables
    for (const auto& table : tables) {
        Container rm = getContainer(table);
        if (rm == NullContainer) return setError(msg);
        rms[table] = rm;
    }
    // TODO: check column names

    // get individual candidates
    vector<vector<Record>> rs;
    for (auto& rm : rms) {
        auto filter = getFilter(expr);
	    vector<Record> results;
	    rm.second.second->query(filter, results);
        rs.push_back(std::move(results));
    }

    // join and filter again
    auto filter = getJoinFilter(expr);
    vector<vector<DValue>> results;
    if (!executeJoin(tables, rs, filter, selectors, results)) return setError(msg);

    // groupby
    if (!groupBy.empty) {
        // TODO
    }

    // accumulation

    msg = cmsg.str();
    return true;
}

bool QueryManager::UseDatabase(const string& name, string& msg) {
    cmsg.str("");
	sysmgr->useDatabase(name);
    msg = cmsg.str();
    return true;
}

bool QueryManager::ShowDatabase(string& msg) {
    cmsg.str("");
	sysmgr->showDatabases();
    msg = cmsg.str();
    return true;
}

bool QueryManager::CreateDatabase(const string& name, string& msg) {
    cmsg.str("");
	sysmgr->createDatabase(name);
    msg = cmsg.str();
    return true;
}

bool QueryManager::DropDatabase(const string& name, string& msg) {
    cmsg.str("");
	sysmgr->dropDatabase(name);
    msg = cmsg.str();
    return true;
}

bool QueryManager::ShowTables(string& msg) {
    cmsg.str("");
	sysmgr->showTables();
    msg = cmsg.str();
    return true;
}

bool QueryManager::DescTable(const string& name, string& msg) {
    cmsg.str("");
	sysmgr->descTable(name);
    msg = cmsg.str();
    return true;
}

bool QueryManager::CreateTable(const string& name, vector<Column>& cols,
		vector<Constraint>& cons, string& msg) {
    cmsg.str("");
	Table& table = sysmgr->createTable(name, cols.size());

    int sz = cols.size() / 8 + 1;
    Column null(42, "NULL", DType::STRING, sz, 0);
    table.addColumn(null);

    vector<int> cids;
    cids.resize(cols.size());
    int i = 0;
	for (auto& col : cols) {
        int cid = table.addColumn(col);
        if (cid == -1) return setError(msg);
		cids[i++] = cid;
    }
	for (auto& con : cons) {
        con.cid = cids[con.cid];
		table.addConstraint(con);
    }
    msg = cmsg.str();
    return true;
}

bool QueryManager::DropTable(const string& name, string& msg) {
    cmsg.str("");
	sysmgr->dropTable(name);
    msg = cmsg.str();
    return true;
}
