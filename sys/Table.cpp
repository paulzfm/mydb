#include "Table.h"

Table NullTable = Table();

Table::Table() {
	this->width = 0;
	this->maxcid = 0;
}

Table::Table(int tid, const std::string& name, int count) {
	this->tid = tid;
	this->name = name;
	// allocation for Nulls
	this->width = (count - 1) / 8 + 1;
	this->maxcid = 0;
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

int Table::getColumnById(int cid) {
	for (unsigned i = 0; i < columns.size(); ++i)
		if (columns[i].cid == cid) return i;
	return -1;
}

int Table::getColumnByName(std::string name) {
	for (unsigned i = 0; i < columns.size(); ++i)
		if (name == std::string(columns[i].name)) return i;
	return -1;
}

Column& Table::getColumn(int index) {
	assert(index >= 0 && index < columns.size());
	return columns[index];
}

bool Table::addColumn(Column& col) {
	// check if column name is unique
	for (auto& c : columns)
		if (c.name == col.name) {
			cerr << "[ERROR] Column " << name << " already exist." << endl;
			return false;
		}

	col.cid = ++maxcid;
	col.offset = this->width;
	this->width += col.size;
	this->columns.push_back(std::move(col));
	return true;
}

// TODO: rearrange data store in this table
bool Table::removeColumn(int cid) {
	for (unsigned i = 0; i < this->columns.size(); ++i)
		if (columns[i].cid == cid) {
			columns.erase(columns.begin() + i);
			return true;
		}
	cerr << "[ERROR] Column not found!" << endl;
	return false;
}

bool Table::addConstraint(Constraint& c) {
	// TODO: check
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
	cout << "------ Columns of Table " << name << " ------" << endl;
	for (auto& col : columns) {
		cout << "| " << col.name << " of type " << col.type << "\t";
		for (auto& cons : constraints) {
			if (cons.cid != col.cid) continue;
			switch (cons.type) {
				case 0:
					cout << " Not Null\t";
					break;
				case 1:
					cout << " Unique\t";
					break;
				case 2:
					cout << " Primary Key\t";
					break;
				case 3:
					cout << " Foreign Key\t";
					break;
				case 4:
					cout << " Check\t";
					break;
				case 5:
					cout << " Default\t";
					break;
			}
		}
		cout << endl;
	}
	cout << endl;
}
