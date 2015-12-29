#include "Table.h"
#include "../query/utils.h"
#include "../util/print.h"

Table NullTable = Table();

Table::Table() {
	this->width = 0;
	this->maxcid = -1;
}

Table::Table(int tid, const std::string& name) {
	this->tid = tid;
	this->name = name;
	// allocation for Nulls
	this->width = 0;
	this->maxcid = -1;
}

int Table::getTid() const {
	return tid;
}

int Table::getWidth() const {
	return width;
}

string Table::getName() const {
	return name;
}

int Table::getColumnById(int cid) const {
	for (unsigned i = 0; i < columns.size(); ++i)
		if (columns[i].cid == cid) return i;
	return -1;
}

int Table::getColumnByName(std::string name) const {
	for (unsigned i = 0; i < columns.size(); ++i)
		if (name == std::string(columns[i].name)) return i;
	return -1;
}

Column& Table::getColumn(int index) {
	assert(index >= 0 && index < columns.size());
	return columns[index];
}

int Table::addColumn(Column& col) {
	// check if column name is unique
	for (auto& c : columns)
		if (c.name == col.name) {
			cmsg << "[ERROR] Column " << name << " already exist." << endl;
			return -1;
		}

	col.cid = ++maxcid;
	col.offset = this->width;
	this->width += col.size;
	this->columns.push_back(std::move(col));
	return maxcid;
}

// TODO: rearrange data store in this table
bool Table::removeColumn(int cid) {
	for (unsigned i = 0; i < this->columns.size(); ++i)
		if (columns[i].cid == cid) {
			columns.erase(columns.begin() + i);
			return true;
		}
	cmsg << "[ERROR] Column not found!" << endl;
	return false;
}

bool Table::addConstraint(Constraint& c) {
	constraints.push_back(std::move(c));
}

bool Table::open(std::ifstream& fin) {
	maxcid = 0;

	string str;
	std::getline(fin, str);
	rapidjson::Document doc;
	doc.Parse(str.c_str());

	// metadatas
	assert(doc.HasMember("name") && doc["name"].IsString());
	name = doc["name"].GetString();
	assert(doc.HasMember("width") && doc["width"].IsInt());
	width = doc["width"].GetInt();

	// read columns
	assert(doc.HasMember("columns") && doc["columns"].IsArray());
	const rapidjson::Value& Cs = doc["columns"];
	for (auto iter = Cs.Begin(); iter != Cs.End(); ++iter) {
		Column column;
		column.unserialize(*iter);
		if (column.cid > maxcid) maxcid = column.cid;
		columns.push_back(std::move(column));
	}

	// read constraints
	assert(doc.HasMember("constraints") && doc["constraints"].IsArray());
	const rapidjson::Value& Ct = doc["constraints"];
	for (auto iter = Ct.Begin(); iter != Ct.End(); ++iter) {
		Constraint constraint;
		constraint.unserialize(rapidjson::Value(*iter, doc.GetAllocator()));
		constraints.push_back(std::move(constraint));
	}

	return true;
}

bool Table::close(std::ofstream& fout) const {
	rapidjson::Document doc(rapidjson::kObjectType);
	rapidjson::Document::AllocatorType& alloc = doc.GetAllocator();

	// write metadatas
	rapidjson::Value vName;
	vName.SetString(name.c_str(), alloc);
	doc.AddMember("name", vName, alloc);
    doc.AddMember("width", width, alloc);

	// write columns
	rapidjson::Value vCol(rapidjson::kArrayType);
	for (auto& col : columns) {
		rapidjson::Value val = col.serialize(doc);
		vCol.PushBack(val, alloc);
	}
	doc.AddMember("columns", vCol, alloc);

	// write constraints
	rapidjson::Value vCon(rapidjson::kArrayType);
	for (auto& con : constraints) {
		rapidjson::Value val = con.serialize(doc);
		vCon.PushBack(val, alloc);
	}
	doc.AddMember("constraints", vCon, alloc);

	rapidjson::StringBuffer buf;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buf);
	doc.Accept(writer);
	string str = buf.GetString();
	fout << str << endl;

	return true;
}

void Table::desc() const {
    std::vector<std::string> heads;
    heads.push_back("Field");
    heads.push_back("Type");
    heads.push_back("Null");
    heads.push_back("Key");
    heads.push_back("Default");
    heads.push_back("Extra");

    std::vector< std::vector<std::string> > rows;
    bool ignore = true;
    for (auto& col : columns) {
        if (ignore) {
            ignore = false;
            continue;
        }

        std::vector<std::string> row;
        row.push_back(col.name);
        row.push_back(DType::toString(col.type, col.size));

        bool attr[7];
        for (int i = 0; i < 7; i++) {
            attr[i] = false;
        }
        for (auto& cons : constraints) {
            if (cons.cid != col.cid) continue;
			attr[cons.type] = true;
        }

        // Null
        if (attr[Constraint::NOT_NULL]) {
            row.push_back("NO");
        } else {
            row.push_back("YES");
        }

        // Key
        if (attr[Constraint::PRIMARY_KEY] && attr[Constraint::FOREIGN_KEY]) {
            row.push_back("PRI FOR");
        } else if (attr[Constraint::PRIMARY_KEY]) {
            row.push_back("PRI");
        } else if (attr[Constraint::FOREIGN_KEY]) {
            row.push_back("FOR");
        } else {
            row.push_back("");
        }

        // Default
        if (attr[Constraint::DEFAULT]) {
            row.push_back("YES");
        } else {
            row.push_back("NULL");
        }

        // Extra: UNIQUE and AUTO_INCREMENT
        if (attr[Constraint::UNIQUE] && attr[Constraint::AUTO_INCREMENT]) {
            row.push_back("UNIQUE AUTO_INCREMENT");
        } else if (attr[Constraint::UNIQUE]) {
            row.push_back("UNIQUE");
        } else if (attr[Constraint::FOREIGN_KEY]) {
            row.push_back("AUTO_INCREMENT");
        } else {
            row.push_back("");
        }

        rows.push_back(row);
    }

    cmsg << tableToString(heads, rows)
         << columns.size() - 1 << " rows in set.\n";
}

bool Table::checkConstraints(const char* rec, RecordManager* rm) {
    DValue null = getColumnValue(rec, 0);
    for (const auto& con : constraints) {
        int mask = 1 << (con.cid % 8);
        switch (con.type) {
            case Constraint::PRIMARY_KEY:
            case Constraint::NOT_NULL: {
                if ((null.data[con.cid / 8] & mask) > 0) {
                    Column& col = columns[getColumnById(con.cid)];
                    cmsg << "[ERROR] Column " << col.name << " is NOT NULL." << endl;
                    return false;
                }
                if (con.type == Constraint::NOT_NULL) break;
            }
            case Constraint::UNIQUE: {
                if ((null.data[con.cid / 8] & mask) == 0) {
                    DValue val = getColumnValue(rec, con.cid);
                    Column& col = columns[getColumnById(con.cid)];
                    auto filter = getFilter(this, getBoolExpr(name, col.name, val));
                    Record r;
                    int qr = rm->queryIndex(col.name, val);
                    if (qr == RecordManager::INDEX_NOT_FOUND) break;
                    if (qr == RecordManager::INDEX_FOUND || rm->queryOne(filter, r)) { // query in index first
                        cmsg << "[ERROR] value in column " << col.name << " is not UNIQUE." << endl;
                        return false;
                    }
                }
                break;
            }
            // TODO
            case Constraint::FOREIGN_KEY: {
                break;
            }
            case Constraint::CHECK: {
                break;
            }
        }
    }
    return true;
}

bool Table::setColumnValue(char* rec, short cid, const DValue& val) const {
    int id = getColumnById(cid);
    const Column& col = columns[id];
    switch (col.type) {
        case DType::BYTE: {
            if (!val.isInt()) return false;
            char v = val.getInt();
            memcpy(rec + col.offset, &v, col.size);
            return true;
        }
        case DType::SHORT: {
            if (!val.isInt()) return false;
            short v = val.getInt();;
            memcpy(rec + col.offset, &v, col.size);
            return true;
        }
        case DType::INT: {
            if (!val.isInt()) return false;
            int v = val.getInt();
            memcpy(rec + col.offset, &v, col.size);
            return true;
        }
        case DType::LONG: {
            if (!val.isInt()) return false;
            int64_t v = val.getInt();
            memcpy(rec + col.offset, &v, col.size);
            return true;
        }
        case DType::FLOAT: {
            if (!val.isReal()) return false;
            float v = val.getReal();
            memcpy(rec + col.offset, &v, col.size);
            return true;
        }
        case DType::DOUBLE: {
            if (!val.isReal()) return false;
            double v = val.getReal();
            memcpy(rec + col.offset, &v, col.size);
            return true;
        }
        case DType::CHAR: {
            if (!val.isInt()) return false;
            char v = val.getInt();
            memcpy(rec + col.offset, &v, col.size);
            return true;
        }
        case DType::STRING: {
            if (!val.isString()) return false;
            memcpy(rec + col.offset, val.data, val.len);
            return true;
        }
    }
}

DValue Table::getColumnValue(const char* rec, short cid) const {
    int id = getColumnById(cid);
    const Column& col = columns[id];
    switch (col.type) {
        case DType::BYTE: {
            char v;
            memcpy(&v, rec + col.offset, col.size);
            return DValue((int64_t)v);
        }
        case DType::SHORT: {
            short v;
            memcpy(&v, rec + col.offset, col.size);
            return DValue((int64_t)v);
        }
        case DType::INT: {
            int v;
            memcpy(&v, rec + col.offset, col.size);
            return DValue((int64_t)v);
        }
        case DType::LONG: {
            int64_t v;
            memcpy(&v, rec + col.offset, col.size);
            return DValue(v);
        }
        case DType::FLOAT: {
            float v;
            memcpy(&v, rec + col.offset, col.size);
            return DValue((double)v);
        }
        case DType::DOUBLE: {
            double v;
            memcpy(&v, rec + col.offset, col.size);
            return DValue(v);
        }
        case DType::CHAR: {
            char v;
            memcpy(&v, rec + col.offset, col.size);
            return DValue((int64_t)v);
        }
        case DType::STRING: {
            string str(rec + col.offset, col.size);
            return DValue(str);
        }
    }
    return DValue();
}
