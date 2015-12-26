#ifndef VALUE_H_
#define VALUE_H_

#include "../util/common.h"
#include "../record/types.h"

class DValue {
public:
    DType type;
    short len;
    char* data;

    DValue();
    DValue(bool val);
    DValue(int64_t val);
    DValue(double val);
    DValue(string& val);
    ~DValue();

    bool isNull() const;
    bool isBool() const;
    bool isInt() const;
    bool isReal() const;
    bool isNum() const;
    bool isString() const;

    bool getBool() const;
    int64_t getInt() const;
    double getReal() const;
    string getString() const;
};

DValue operator == (const DValue& a, const DValue& b);
DValue operator != (const DValue& a, const DValue& b);
DValue operator < (const DValue& a, const DValue& b);
DValue operator > (const DValue& a, const DValue& b);
DValue operator <= (const DValue& a, const DValue& b);
DValue operator >= (const DValue& a, const DValue& b);
DValue operator && (const DValue& a, const DValue& b);
DValue operator || (const DValue& a, const DValue& b);
DValue operator ! (const DValue& a);
DValue operator + (const DValue& a, const DValue& b);
DValue operator - (const DValue& a, const DValue& b);
DValue operator * (const DValue& a, const DValue& b);
DValue operator / (const DValue& a, const DValue& b);
DValue operator % (const DValue& a, const DValue& b);

#endif 
