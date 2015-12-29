#include "Database.h"
#include "unistd.h"

Database NullDatabase = Database();

int Database::getTableByName(const string& name) const {
	for (unsigned i = 0; i < tables.size(); ++i)
		if (tables[i].name == name)
			return i;
	return -1;
}

Table& Database::getTable(int index) {
    assert(index >= 0 && index < tables.size());
    return tables[index];
}

Database::Database() {
	maxtid = 0;
}

Database::Database(const std::string& name) {
	this->name = name;
	maxtid = 0;
}

bool Database::open(const string& name) {
	this->name = name;
	this->maxtid = 0;

	std::ifstream fin((name + "/" + TABLELIST_FILE).c_str());
	if (fin.is_open()) {
		string str;
		std::getline(fin, str);

		rapidjson::Document d;
		d.Parse(str.c_str());

		assert(d.HasMember("name") && d["name"].IsString());
		this->name = d["name"].GetString();

		assert(d.HasMember("table_num") && d["table_num"].IsInt());
		int nTb = d["table_num"].GetInt();
		for (int i = 0; i < nTb; ++i) {
			Table table;
			table.open(fin);
			if (table.getTid() > maxtid) maxtid = table.getTid();
			tables.push_back(std::move(table));
		}

		fin.close();
		return true;
	} else {
		cmsg << "[ERROR] table list of database " << name << " file not found." << endl;
		return false;
	}
}

bool Database::close() const {
	std::ofstream fout((name + "/" + TABLELIST_FILE).c_str());
	if (!fout.is_open()) return false;
	rapidjson::Document doc(rapidjson::kObjectType);
	rapidjson::Document::AllocatorType& alloc = doc.GetAllocator();

	rapidjson::Value vName;
	vName.SetString(name.c_str(), alloc);
	doc.AddMember("name", vName, alloc);
	doc.AddMember("table_num", (int)(tables.size()), alloc);

	rapidjson::StringBuffer buf;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buf);
	doc.Accept(writer);
	string str = buf.GetString();
	fout << str << endl;

	for (const auto& table : tables) {
		table.close(fout);
	}
	fout.close();
	return true;
}

void Database::showTables() const {
	std::cout << "-- LIST OF TABLES" << endl;
	for (const auto& table : tables) {
		std::cout << ">> " << table.name << endl;
	}
}

void Database::descTable(const std::string& name) const {
	int tid = getTableByName(name);
	if (tid == -1) {
		cmsg << "[ERROR] Table " << name << " not exists." << endl;
		return;
	}

	tables[tid].desc();
}

Table& Database::createTable(const string& name, int count) {
	// check if name is unique
	int tid = getTableByName(name);
	if (tid != -1) {
		cmsg << "[ERROR] Table " << name << " already exist." << endl;
		return NullTable;
	}

	Table table(++maxtid, name);
	tables.push_back(std::move(table));
	return tables.back();
}

void Database::dropTable(const std::string& name) {
	int tid = getTableByName(name);
	if (tid == -1) {
		cmsg<< "[ERROR] Table " << name << " not exists." << endl;
		return;
	}

	tables.erase(tables.begin() + tid);
}
