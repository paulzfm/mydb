#include "Constraint.h"

Constraint::Constraint() {}

Constraint::Constraint(short _cid, const string& _name, char _type,
		Value& _data) {
	cid = _cid;
	name = _name;
	type = _type;
	data = _data;
}

Value Constraint::serialize(Document& doc) const {
	Document::AllocatorType& alloc = doc.GetAllocator();

	Value value(kObjectType), vName;
	vName.SetString(name.c_str(), alloc);
	value.AddMember("cid", (int)cid, alloc);
	value.AddMember("name", vName, alloc);
	value.AddMember("type", (int)type, alloc);
	value.AddMember("data", Value(data, alloc), alloc);

	return value;
}

void Constraint::unserialize(Value value) {
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
