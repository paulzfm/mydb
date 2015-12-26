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
    Database& db = sysmgr->getCurrentDB();
    int tid = db.getTableByName(name);
    if (tid == -1) {
        cmsg << "[ERROR] Table " + name + " not found!" << endl;
        return NullContainer;
    }
    Table* table = &db.getTable(tid);

	if (tables.find(idx) == tables.end()) {
		string path = Configuration::conf()->basepath + db.name;
		path += '/' + name + ".dat";
		RecordManager* rm = new RecordManager(path);
		tables[idx] = rm;
        return make_pair(table, rm);

	} else {
		return make_pair(table, tables[idx]);
	}
}

std::function<bool(const Record&)> QueryManager::getFilter(BoolExpr* expr) {
    return [this, expr] (const Record& rec) {
        Evaluator eval(this, rec.data);
        expr->accept(&eval);
        return eval.getValues().back().getBool();
    };
}

std::function<bool(const vector<DValue>&)> QueryManager::getJoinFilter(BoolExpr* expr) {
    return [this, expr] (const vector<DValue>&) {
        return true;
    };
}

QueryManager::QueryManager(SystemManager* sysmgr_) : sysmgr(sysmgr_) {
}

QueryManager::~QueryManager() {
	for (auto tb : tables)
		delete tb.second;
	tables.clear();
}

bool QueryManager::Insert(const string& table, vector<Value*>& data, string& msg) {
	Container rm = getContainer(table);
	if (rm == NullContainer) return setError(msg);
    
    // NOTE: null column in table
    if (rm.first->columns.size() != data.size() + 1) {
        cmsg << "[ERROR] column number in record mismatched." << endl;
        return setError(msg);
    }

    // fill map
    unordered_map<string, Value*> dat;
    for (int i = 1; i <= data.size(); ++i) {
        dat[rm.first->columns[i].name] = data[i - 1];
    }

    return Insert(table, dat, msg);
}

void set(char* rec, char* null, short cid, Value* val, int size) {
    switch (val->kind) {
        case Value::VALUE_INT:
            switch (size) {
                case 1: {
                    char v1 = atoi(val->val.c_str());
                    memcpy(rec, &v1, 1);
                    break;
                }
                case 2: {
                    short v2 = atoi(val->val.c_str());
                    memcpy(rec, &v2, 2);
                    break;
                }
                case 4: {
                    int v4 = atoi(val->val.c_str());
                    memcpy(rec, &v4, 4);
                    break;
                }
                case 8: {
                    int64_t v8 = atoi(val->val.c_str());
                    memcpy(rec, &v8, 8);
                    break;
                }
            }
            break;
        case Value::VALUE_REAL:
            if (size == 4) {
                float v = atof(val->val.c_str());
                memcpy(rec, &v, 4);
            } else { // size == 8
                double v = atof(val->val.c_str());
                memcpy(rec, &v, 8);
            }
            break;
        case Value::VALUE_STRING:
            strncpy(rec, val->val.c_str(), size - 1);
            break;
        case Value::VALUE_NULL:
            null[cid / 8] &= ~(1 << (cid % 8));
            break;
    }
}

bool QueryManager::Insert(const string& table, unordered_map<string, Value*>& data, string& msg) {
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
            null[col.cid / 8] &= ~(1 << (col.cid % 8));
            set(buf + col.offset, null, col.cid, iter->second, col.size);
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
	if (rm == NullContainer) return setError(msg);

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

bool QueryManager::join(const vector<string>& tables,
        const vector<vector<Record>>& rs,
        std::function<bool(const vector<DValue>&)> filter,
        const Selectors& selectors,
        vector<vector<DValue>>& results) {

    std::function<bool(int, vector<DValue>&)> recur_func;
    recur_func = [&] (int level, vector<DValue>& rec) {
        if (level == tables.size()) {
            if (filter(rec)) results.push_back(rec);
            return true;

        } else {
            string tbName = tables[level];
            Table* table = getContainer(tbName).first;
            int sz = rec.size();
            for (int i = 0; i < rs[level].size(); ++i) {
                const Record& r = rs[level][i];
                if (selectors.all) {
                    for (int idx = 0; idx < table->columns.size(); ++idx) {
                        Column& col = table->getColumn(idx);
                        rec.push_back(table->getColumnValue(r.data, col.cid));
                    }
                } else {
                    int i = -1;
                    for (const Selector* sel : *selectors.selectors) {
                        ++i;
                        if (sel->col->tb != tbName) continue;
                        int idx = table->getColumnByName(sel->col->col);
                        Column& col = table->getColumn(idx);
                        rec[i] = table->getColumnValue(r.data, col.cid);
                    }
                }
                if (!recur_func(level + 1, rec)) return false;
                if (selectors.all) rec.resize(sz);
            }
            return true;
        }
    };
    vector<DValue> rec;
    if (!selectors.all) rec.resize(selectors.selectors->size());
    return recur_func(0, rec);
}

bool QueryManager::group(const GroupBy& groupby,
        const Selectors& selectors,
        vector<vector<DValue>>& input,
        vector<vector<vector<DValue>>>& output) {

    int sz = -1, idx = -1;
    for (const Selector* sel : *selectors.selectors) {
        ++sz;
        if (sel->col->tb != groupby.tb || sel->col->col != groupby.col) continue;
        idx = sz;
        break;
    }
    if (idx == -1) {
        cmsg << "[ERROR] column not found in GROUP BY clause." << endl;
        return false;
    }

    int cnt = 0;
    std::map<DValue, int, DValueLT> V;
    for (const auto& rec : input) {
        if (V.find(rec[idx]) == V.end()) {
            V[rec[idx]] = cnt++;
        }
        output[V[rec[idx]]].push_back(rec);
    }
    return true;
}

bool QueryManager::aggregate(const Selectors& selectors,
        const GroupBy& groupby,
        vector<vector<vector<DValue>>>& input,
        vector<vector<DValue>>& output) {

    for (const Selector* sel : *selectors.selectors) {
        if (sel->col->tb != groupby.tb || sel->col->col != groupby.col) {
            if (sel->func == Selector::FUNC_NULL) {
                cmsg << "[ERROR] mixed aggregate and non-aggregate columns." << endl;
                return false;
            }
        }
    }

    for (int gid = 0; gid < input.size(); ++gid) {
        vector<DValue> rec;
        int i = -1;
        for (const Selector* sel : *selectors.selectors) {
            ++i;
            if (sel->col->tb != groupby.tb || sel->col->col != groupby.col) {
                DValue val = input[gid][0][i];
                for (int id = 1; id < input[gid].size(); ++id) {
                    switch (sel->func) {
                        case Selector::FUNC_SUM:
                        case Selector::FUNC_AVG:
                            val = val + input[gid][id][i];
                            break;
                        case Selector::FUNC_MAX:
                            if ((val < input[gid][id][i]).getBool())
                                val = input[gid][id][i];
                            break;
                        case Selector::FUNC_MIN:
                            if ((val > input[gid][id][i]).getBool())
                                val = input[gid][id][i];
                            break;
                    }
                }
                if (sel->func == Selector::FUNC_AVG)
                    val = val / DValue((int64_t)input[gid].size());
                rec.push_back(val);

            } else {
                // all the same, take the first one
                rec.push_back(input[gid][0][i]);
            }
        }
        output.push_back(rec);
    }

    return true;
}

bool QueryManager::print(const Selectors& selectors,
        const vector<vector<DValue>>& results) {
    cmsg << "|";
    for (const Selector* sel : *selectors.selectors)
        cmsg << sel->col->tb << '.' << sel->col->col << "\t|";
    cmsg << "|" << endl;

    for (const auto& rec : results) {
        cmsg << "|";
        for (const auto& val : rec) {
            val.print();
            cmsg << "\t|";
        }
        cmsg << "|" << endl;
    }
}

bool QueryManager::Select(const vector<string>& tables, const Selectors* selectors,
        BoolExpr* expr, const GroupBy* groupBy, string& msg) {
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
    vector<vector<DValue>> jres;
    if (!join(tables, rs, filter, *selectors, jres)) return setError(msg);

    // groupby
    vector<vector<vector<DValue>>> gres;
    if (!groupBy->empty) {
        if (!group(*groupBy, *selectors, jres, gres)) return setError(msg);
    } else {
        gres.push_back(std::move(jres));
    }

    // aggregate
    vector<vector<DValue>> results;
    if (!aggregate(*selectors, *groupBy, gres, results)) return setError(msg);

    // print result
    if (!print(*selectors, results)) return setError(msg);

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
