#ifndef VALUE_H_
#define VALUE_H_

#include <string>

struct Value {
	int len;
	int type;
	char* data;

	Value();
	Value(int8_t);
	Value(int16_t);
	Value(int32_t);
	Value(int64_t);
	Value(float);
	Value(double);
	Value(char);
	Value(std::string);

	~Value();
};

extern Value NullValue;

#endif 
