#include "Column.h"

Column NullColumn = Column(-1, "Null Column", 0, 0, 0);

Column::Column() {}

Column::Column(int _cid, const string& _name, char _type, short _size, short _offset)
		: cid(_cid), name(_name), type(_type), size(_size), offset(_offset) {
}

Value Column::serialize(Document& doc) const {
	Document::AllocatorType& alloc = doc.GetAllocator();

	Value value(kObjectType), vName;
	vName.SetString(name.c_str(), alloc);

	value.AddMember("cid", (int)cid, alloc);
	value.AddMember("name", vName, alloc);
	value.AddMember("type", (int)type, alloc);
	value.AddMember("size", (int)size, alloc);
	value.AddMember("offset", (int)offset, alloc);

	return value;
}

void Column::unserialize(const Value& value) {
	assert(value.IsObject());

	assert(value.HasMember("cid") && value["cid"].IsInt());
	cid = value["cid"].GetInt();
	assert(value.HasMember("name") && value["name"].IsString());
	name = value["name"].GetString();
	assert(value.HasMember("type") && value["type"].IsInt());
	type = value["type"].GetInt();
	assert(value.HasMember("size") && value["size"].IsInt());
	size = value["size"].GetInt();
	assert(value.HasMember("offset") && value["offset"].IsInt());
	offset = value["offset"].GetInt();
}
