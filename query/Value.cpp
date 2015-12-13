#include "Value.h"

Value NullValue;

Value::Value() {
	type = len = data = 0;
}

Value::Value(int8_t x) {
	len = 1;
	type = DType::BYTE;
	data = new char[1];
	memcpy(data, &x, 1);
}

Value::Value(int16_t x) {
	len = 2;
	type = DType::SHORT;
	data = new char[2];
	memcpy(data, &x, 2);
}

Value::Value(int32_t x) {
	len = 4;
	type = DType::INT;
	data = new char[4];
	memcpy(data, &x, 4);
}

Value::Value(int64_t x) {
	len = 8;
	type = DType::LONG;
	data = new char[8];
	memcpy(data, &x, 8);
}

Value::Value(float x) {
	len = 4;
	type = DType::FLOAT;
	data = new char[4];
	memcpy(data, &x, 4);
}

Value::Value(double x) {
	len = 8;
	type = DType::DOUBLE;
	data = new char[8];
	memcpy(data, &x, 8);
}

Value::Value(char x) {
	len = 1;
	type = DType::CHAR;
	data = new char[1];
	memcpy(data, &x, 1);
}

Value::Value(std::string x) {
	len = x.length() + 1;
	type = DType::STRING;
	data = new char[len];
	strcpy(data, x.c_str());
}

Value::~Value() {
	delete [] data;
}
