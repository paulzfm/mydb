#include "QueryManager.h"
#include "Evaluator.h"
#include "../util/print.h"
#include "../record/types.h"

Container NullContainer = make_pair((Table*)NULL, (RecordManager*)NULL);

bool setError(string& msg) {
    msg = cmsg.str();
    return false;
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
		string path = db.name + '/' + name + ".dat";
		RecordManager* rm = new RecordManager(path);
		tables[idx] = rm;
        return make_pair(table, rm);

	} else {
		return make_pair(table, tables[idx]);
	}
}

QueryManager::QueryManager(SystemManager* sysmgr_) : sysmgr(sysmgr_) {
}

QueryManager::~QueryManager() {
	for (const auto& tb : tables)
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
            strncpy(rec, val->val.c_str(), size);
            break;
        case Value::VALUE_NULL:
            null[cid / 8] |= (1 << (cid % 8));
            break;
    }
}

bool compatible(int colType, int valType) {
    if (valType == Value::VALUE_NULL) return true;
    return (((colType == DType::BYTE || colType == DType::SHORT
                    || colType == DType::INT || colType == DType::LONG)
                && valType == Value::VALUE_INT) ||
            ((colType == DType::FLOAT || colType == DType::DOUBLE)
                && valType == Value::VALUE_REAL) ||
            (colType == DType::STRING && valType == Value::VALUE_STRING) ||
            (colType == DType::BOOL && (valType == Value::VALUE_TRUE || valType == Value::VALUE_FALSE)));
            
}

bool QueryManager::Insert(const string& table, unordered_map<string, Value*>& data, string& msg) {
    cmsg.str("");
	Container rm = getContainer(table);
	if (rm == NullContainer) return setError(msg);
	
	char* buf = new char[rm.first->getWidth()];
	memset(buf, 0, sizeof(char) * rm.second->width());

    // intialize null column to all 1
    Column& col = rm.first->getColumn(rm.first->getColumnById(0));
    memset(buf + col.offset, -1, col.size);
    char* null = buf + col.offset;

    // fill default values
    for (auto& con : rm.first->constraints) {
        if (con.type == Constraint::DEFAULT) {
            rm.first->setColumnValue(buf, con.cid, DValue(con.data));
            null[con.cid / 8] &= ~(1 << (con.cid % 8));
        }
        if (con.type == Constraint::AUTO_INCREMENT) {
            rm.first->setColumnValue(buf, con.cid, DValue(con.data));
        }
    }

	// fill data
	for (auto& col : rm.first->columns) {
		auto iter = data.find(col.name);
		if (iter != data.end()) {
            // type check
            if (!compatible(col.type, iter->second->kind)) {
                cmsg << "[ERROR] data of column " << col.name << " is not compatible." << endl;
                return setError(msg);
            }
            null[col.cid / 8] &= ~(1 << (col.cid % 8));
            set(buf + col.offset, null, col.cid, iter->second, col.size);
            data.erase(iter);
        }
	}
    
    // check constraints
    if ( ! rm.first->checkConstraints(buf, rm.second) ) return setError(msg);

    for (auto& p : data) {
        cmsg << "[WARNING] Column " << p.first << " not found, ignored" << endl;
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
    auto filter = getFilter(rm.first, where);
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
    auto filter = getFilter(rm.first, expr);
	vector<Record> results;
	rm.second->query(filter, results);

	// update
    for (auto& p : data) {
        int cid = rm.first->getColumnByName(p.first);
        if (cid == -1) return setError(msg);

        for (auto& rec : results) {
            unordered_map<string, DValue> values;
            string prefix = table + ".";
            for (const auto& col : rm.first->columns) {
                DValue val = rm.first->getColumnValue(rec.data, col.cid);
                values[prefix + col.name] = val;
            }
            Evaluator eval(values, table);
            p.second->accept(&eval);
            rm.first->setColumnValue(rec.data, cid, eval.getValues().back());
        }
    }

    for (auto& rec : results) {
        // check constraints
        if ( ! rm.first->checkConstraints(rec.data, rm.second) ) return setError(msg);
    }

	for (auto& rec : results)
		rm.second->replace(rec.page, rec.offset, rec.data);

    msg = cmsg.str();
    return true;
}

bool QueryManager::join(const vector<string>& tables,
        const vector<vector<Record>>& rs,
        std::function<bool(const unordered_map<string, DValue>&)> filter,
        const Selectors& selectors,
        vector<unordered_map<string, DValue>>& results) {

    std::function<bool(int, unordered_map<string, DValue>&)> recur_func;
    string prefix = "";
    if (tables.size() == 1) prefix = tables[0] + ".";
    recur_func = [&] (int level, unordered_map<string, DValue>& rec) {
        if (level == tables.size()) {
            if (filter(rec)) {
                unordered_map<string, DValue> v;
                // generate queried results
                if (selectors.all) {
                    results.push_back(rec);
                    return true;
                }
                for (const Selector* sel : *selectors.selectors) {
                    string tbName = sel->col->tb;
                    if (tbName == "") {
                        if (tables.size() == 1) tbName = tables[0];
                        else {
                            cmsg << "[ERROR] table name should be provided when SELECT from multiple tables." << endl;
                            return false;
                        }
                    }
                    Table* table = getContainer(tbName).first;
                    int idx = table->getColumnByName(sel->col->col);
                    Column& col = table->getColumn(idx);
                    string name = tbName + "." + sel->col->col;
                    v[name] = rec[name];
                }
                results.push_back(v);
            }
            return true;

        } else {
            string tbName = tables[level];
            Table* table = getContainer(tbName).first;
            int sz = rec.size();
            for (int i = 0; i < rs[level].size(); ++i) {
                const Record& r = rs[level][i];
                DValue null = table->getColumnValue(r.data, 0);
                for (int idx = 1; idx < table->columns.size(); ++idx) {
                    Column& col = table->getColumn(idx);
                    string name = tbName + "." + col.name;
                    if ((null.data[idx / 8] & (1 << (idx % 8))) > 0) {
                        rec[name] = DValue();
                    } else {
                        rec[name] = table->getColumnValue(r.data, col.cid);
                    }
                }
                /*} else {
                    int i = -1;
                    for (const Selector* sel : *selectors.selectors) {
                        ++i;
                        if (sel->col->tb != tbName) continue;
                        int idx = table->getColumnByName(sel->col->col);
                        Column& col = table->getColumn(idx);
                        rec[i] = table->getColumnValue(r.data, col.cid);
                    }
                }*/
                if (!recur_func(level + 1, rec)) return false;

                // backtrace
                for (int idx = 1; idx < table->columns.size(); ++idx) {
                    Column& col = table->getColumn(idx);
                    string name = tbName + "." + col.name;
                    rec.erase(name);
                }
            }
            return true;
        }
    };
    unordered_map<string, DValue> rec;
    return recur_func(0, rec);
}

bool QueryManager::group(const GroupBy& groupby,
        const Selectors& selectors,
        const vector<string>& tables,
        vector<unordered_map<string, DValue>>& input,
        vector<vector<unordered_map<string, DValue>>>& output) {

    string name = groupby.tb + "." + groupby.col;
    int sz = -1, idx = -1;
    if (selectors.all) {
        idx = 0;
    } else {
        for (const Selector* sel : *selectors.selectors) {
            ++sz;
            string cname = sel->col->tb + "." + sel->col->col;
            if (sel->col->tb == "") cname = tables[0] + cname;
            if (cname != name) continue;
            idx = sz;
            break;
        }
    }
    if (idx == -1) {
        cmsg << "[ERROR] column not found in GROUP BY clause." << endl;
        return false;
    }

    int cnt = 0;
    std::map<DValue, int, DValueLT> V;
    for (auto& rec : input) {
        if (V.find(rec[name]) == V.end()) {
            V[rec[name]] = cnt++;
            output.resize(cnt);
        }
        output[V[rec[name]]].push_back(rec);
    }
    return true;
}

bool QueryManager::aggregate(const Selectors& selectors,
        const GroupBy& groupby,
        vector<vector<unordered_map<string, DValue>>>& input,
        vector<unordered_map<string, DValue>>& output) {

    bool agg = false, nagg = false;

    if (!selectors.all) {
        for (const Selector* sel : *selectors.selectors) {
            if (sel->col->tb == groupby.tb && sel->col->col == groupby.col) {
                if (sel->func != Selector::FUNC_NULL) {
                    cmsg << "[ERROR] cannot apply aggregate function on GROUP BY column." << endl;
                    return false;
                }
            } else {
                if (sel->func == Selector::FUNC_NULL) nagg = true;
                else agg = true;
            }
        }
    }
    if (agg && nagg) {
        cmsg << "[ERROR] mixed aggregate and non-aggregate columns." << endl;
        return false;
    }

    if (!agg) {
        if (!groupby.empty) {
            cmsg << "[WARNING] ignore GROUP BY clause without aggregate function." << endl;
        }
        for (const auto& x : input)
            for (const auto& y : x)
                output.push_back(y);
        return true;
    }

    for (int gid = 0; gid < input.size(); ++gid) {
        unordered_map<string, DValue> rec;
        for (const Selector* sel : *selectors.selectors) {
            string name = sel->col->tb + "." + sel->col->col;
            if (sel->col->tb != groupby.tb || sel->col->col != groupby.col) {
                DValue val = input[gid][0][name];
                for (int id = 1; id < input[gid].size(); ++id) {
                    DValue tmp = input[gid][id][name];
                    switch (sel->func) {
                        case Selector::FUNC_SUM:
                        case Selector::FUNC_AVG:
                            val = val + tmp;
                            break;
                        case Selector::FUNC_MAX:
                            if ((val < tmp).getBool())
                                val = tmp;
                            break;
                        case Selector::FUNC_MIN:
                            if ((val > tmp).getBool())
                                val = tmp;
                            break;
                    }
                }
                if (sel->func == Selector::FUNC_AVG)
                    val = val / DValue((double)input[gid].size());
                rec[name] = val;

            } else {
                // all the same, take the first one
                rec[name] = input[gid][0][name];
            }
        }
        output.push_back(rec);
    }

    return true;
}

bool QueryManager::print(const Selectors& selectors,
        const vector<string>& tables,
        const vector<unordered_map<string, DValue>>& results) {
    vector<string> heads;
    vector<vector<string>> data;

    if (results.size() == 0) {
        cmsg << "Empty set." << endl;
        return true;
    }

    if (selectors.all) {
        bool first = true;
        for (const auto& rec : results) {
            vector<string> row;
            if (first) {
                for (const auto& p : rec) {
                    row.push_back(p.second.printToString());
                    heads.push_back(p.first);
                }
                first = false;

            } else {
                for (const auto& name : heads)
                    row.push_back(rec.find(name)->second.printToString());
            }
            data.push_back(row);
        }
    } else {

        for (const Selector* sel : *selectors.selectors) {
            if (sel->col->tb == "") heads.push_back(sel->col->col);
            else heads.push_back(sel->col->tb + "." + sel->col->col);
        }

        for (const auto& rec : results) {
            vector<string> row;
            for (const Selector* sel : *selectors.selectors) {
                string name;
                if (sel->col->tb == "") name = tables[0] + "." + sel->col->col;
                else name = sel->col->tb + "." + sel->col->col;
                row.push_back(rec.find(name)->second.printToString());
            }
            data.push_back(row);
        }
    }
    cmsg << tableToString(heads, data) << endl;
    cmsg << data.size() << " records found in total." << endl;
    return true;
}

bool QueryManager::Select(const vector<string>& tables, Selectors* selectors,
        BoolExpr* expr, GroupBy* groupBy, string& msg) {
    cmsg.str("");
    unordered_map<string, Container> rms;

    // get tables
    for (const auto& table : tables) {
        Container rm = getContainer(table);
        if (rm == NullContainer) return setError(msg);
        rms[table] = rm;
    }

    // check
    if (!groupBy->empty && groupBy->tb == "") {
        if (tables.size() > 1) {
            cmsg << "[ERROR] missing table name in GROUP BY clause." << endl;
            return setError(msg);
        } else {
            groupBy->tb = tables[0];
        }
    }

    // TODO: check column names
    /*
    ColumnChecker cc(tables, rms);
    if (!expr->accept(&cc)) {
        cmsg << "[ERROR] Evaluation error at " << cc.error << ".";
        return false;
    }*/

    // get individual candidates
    vector<vector<Record>> rs;
    for (const auto& table : tables) {
        auto rm = rms[table];
        auto filter = getFilter(rm.first, expr);
	    vector<Record> results;
	    rm.second->query(filter, results);
        rs.push_back(std::move(results));
    }

    // join and filter again
    auto filter = getJoinFilter(expr);
    vector<unordered_map<string, DValue>> jres;
    if (!join(tables, rs, filter, *selectors, jres)) return setError(msg);

    // groupby
    vector<vector<unordered_map<string, DValue>>> gres;
    if (!groupBy->empty) {
        if (!group(*groupBy, *selectors, tables, jres, gres)) return setError(msg);
    } else {
        gres.push_back(std::move(jres));
    }

    // aggregate
    vector<unordered_map<string, DValue>> results;
    if (!aggregate(*selectors, *groupBy, gres, results)) return setError(msg);

    // print result
    if (!print(*selectors, tables, results)) return setError(msg);

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
    if (table.name == "") return setError(msg);

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

    string path = sysmgr->dbs[sysmgr->dbid].name + '/' + name + ".dat";
    RecordManager *rm = new RecordManager(path, table.getWidth());
    delete rm;

    msg = cmsg.str();
    return true;
}

bool QueryManager::DropTable(const string& name, string& msg) {
    cmsg.str("");
	sysmgr->dropTable(name);
    msg = cmsg.str();
    return true;
}
