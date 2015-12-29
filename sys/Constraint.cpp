#include "Constraint.h"

Constraint::Constraint() {}

Constraint::Constraint(const Constraint& con)
    : cid(con.cid), name(con.name), type(con.type), data(con.data, doc.GetAllocator()) {   
}

Constraint::Constraint(short _cid, const string& _name, char _type,
		const rapidjson::Value& _data) : data(_data, doc.GetAllocator()) {
	cid = _cid;
	name = _name;
	type = _type;
}

void Constraint::setData(const rapidjson::Value& data) {
    this->data = rapidjson::Value(data, doc.GetAllocator());
}

rapidjson::Value Constraint::getData() const {
    return rapidjson::Value(data, doc.GetAllocator());
}

rapidjson::Value Constraint::serialize(rapidjson::Document& doc) const {
	rapidjson::Document::AllocatorType& alloc = doc.GetAllocator();

	rapidjson::Value value(rapidjson::kObjectType), vName;
	vName.SetString(name.c_str(), alloc);
	value.AddMember("cid", (int)cid, alloc);
	value.AddMember("name", vName, alloc);
	value.AddMember("type", (int)type, alloc);
	value.AddMember("data", rapidjson::Value(data, alloc), alloc);

	return value;
}

void Constraint::unserialize(rapidjson::Value value) {
	assert(value.IsObject());

	assert(value.HasMember("cid") && value["cid"].IsInt());
	cid = value["cid"].GetInt();
	assert(value.HasMember("name") && value["name"].IsString());
	name = value["name"].GetString();
	assert(value.HasMember("type") && value["type"].IsInt());
	type = value["type"].GetInt();
	assert(value.HasMember("data"));
	data = value["data"];
}
