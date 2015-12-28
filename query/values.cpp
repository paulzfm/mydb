#include "values.h"

DValue::DValue() : type(0) {
    data = NULL;
}

DValue::DValue(const DValue& a) : type(a.type), len(a.len) {
    if (a.data != NULL) {
        int l = len + (type.ident == DType::STRING ? 1 : 0);
        data = new char[l];
        memcpy(data, a.data, l);
    } else
        data = NULL;
}

DValue::DValue(const rapidjson::Value& a) : type(0) {
    // TODO
    if (a.IsInt()) {
        type = DType(DType::INT);
    }
    if (a.IsDouble()) type = DType(DType::DOUBLE);
    if (a.IsString()) type = DType(DType::STRING, a.GetStringLength());
}

DValue::DValue(bool val) : type(DType::BYTE) {
    data = new char[1];
    len = 1;
    *data = val ? 1 : 0;
}

DValue::DValue(int64_t val) : type(DType::LONG) {
    data = new char[8];
    len = 8;
    memcpy(data, &val, 8);
}

DValue::DValue(double val) : type(DType::DOUBLE) {
    data = new char[8];
    len = 8;
    memcpy(data, &val, 8);
}

DValue::DValue(string& val) : type(DType::STRING, val.length()) {
    len = val.length();
    data = new char[len + 1];
    memcpy(data, val.c_str(), len);
    data[len] = '\0';
}

DValue::~DValue() {
    delete [] data;
}

DValue& DValue::operator = (const DValue& a) {
    type = a.type;
    len = a.len;
    if (data) delete [] data;
    if (a.data == NULL) {
        data = NULL;
        return *this;
    }
    int l = len + (type.ident == DType::STRING ? 1 : 0);
    data = new char[l];
    memcpy(data, a.data, l);
    return *this;
}

void DValue::setMiss() {
    type = 1;
}

bool DValue::isNull() const {
    return type.ident == 0;
}

bool DValue::isMiss() const {
    return type.ident == 1;
}

bool DValue::isBool() const {
    return type.ident == DType::BOOL;
}

bool DValue::isInt() const {
    return type.ident == DType::BYTE || type.ident == DType::SHORT ||
        type.ident == DType::INT || type.ident == DType::LONG;
}

bool DValue::isReal() const {
    return type.ident == DType::FLOAT || type.ident == DType::DOUBLE;
}

bool DValue::isNum() const {
    return isInt() || isReal();
}

bool DValue::isString() const {
    return type.ident == DType::STRING;
}

bool DValue::getBool() const {
    return *data == 1;
}

int64_t DValue::getInt() const {
    int64_t val;
    memcpy(&val, data, 8);
    return val;
}

double DValue::getReal() const {
    if (isReal()) {
        double val;
        memcpy(&val, data, 8);
        return val;
    } else {
        int64_t val;
        memcpy(&val, data, 8);
        return val;
    }
}

string DValue::getString() const {
    return string(data);
}

string DValue::printToString() const {
    if (isNull()) return string("NULL");
    if (isInt()) return std::to_string(getInt());
    if (isReal()) return std::to_string(getReal());
    if (isBool()) return getBool() ? string("TRUE") : string("FALSE");
    if (isString()) return string("'") + getString() + "'";
}

DValue operator == (const DValue& a, const DValue& b) {
    if (a.isMiss() || b.isMiss()) return DValue(true);
    if (a.isNull() || b.isNull()) return DValue(false);
    if (a.isBool()) return DValue(a.getBool() == b.getBool());
    if (a.isInt()) return DValue(a.getInt() == b.getInt());
    if (a.isReal()) return DValue(a.getReal() == b.getReal());
    if (a.isString()) return DValue(a.getString() == b.getString());
    return DValue(false);
}

DValue operator != (const DValue& a, const DValue& b) {
    if (a.isMiss() || b.isMiss()) return DValue(true);
    if (a.isNull() || b.isNull()) return DValue(false);
    if (a.isBool() && b.isBool()) return DValue(a.getBool() != b.getBool());
    if (a.isInt() && b.isInt()) return DValue(a.getInt() != b.getInt());
    if (a.isReal() && b.isReal()) return DValue(a.getReal() != b.getReal());
    return DValue(false);
}

DValue operator < (const DValue& a, const DValue& b) {
    if (a.isMiss() || b.isMiss()) return DValue(true);
    if (a.isNull() || b.isNull()) return DValue(false);
    assert(a.isNum() && b.isNum());
    if (a.isInt() && b.isInt()) return DValue(a.getInt() < b.getInt());
    else return DValue(a.getReal() < b.getReal());
}

DValue operator > (const DValue& a, const DValue& b) {
    if (a.isMiss() || b.isMiss()) return DValue(true);
    if (a.isNull() || b.isNull()) return DValue(false);
    assert(a.isNum() && b.isNum());
    if (a.isInt() && b.isInt()) return DValue(a.getInt() > b.getInt());
    else return DValue(a.getReal() > b.getReal());
}

DValue operator <= (const DValue& a, const DValue& b) {
    if (a.isMiss() || b.isMiss()) return DValue(true);
    if (a.isNull() || b.isNull()) return DValue(false);
    assert(a.isNum() && b.isNum());
    if (a.isInt() && b.isInt()) return DValue(a.getInt() <= b.getInt());
    else return DValue(a.getReal() <= b.getReal());
}

DValue operator >= (const DValue& a, const DValue& b) {
    if (a.isMiss() || b.isMiss()) return DValue(true);
    if (a.isNull() || b.isNull()) return DValue(false);
    assert(a.isNum() && b.isNum());
    if (a.isInt() && b.isInt()) return DValue(a.getInt() >= b.getInt());
    else return DValue(a.getReal() >= b.getReal());
}

DValue operator && (const DValue& a, const DValue& b) {
    if (a.isMiss() || b.isMiss()) return DValue(true);
    if (a.isNull() || b.isNull()) return DValue(false);
    assert(a.isBool() && b.isBool());
    return DValue(a.getBool() && b.getBool());
}

DValue operator || (const DValue& a, const DValue& b) {
    if (a.isMiss() || b.isMiss()) return DValue(true);
    if (a.isNull() || b.isNull()) return DValue(false);
    assert(a.isBool() && b.isBool());
    return DValue(a.getBool() || b.getBool());
}

DValue operator ! (const DValue& a) {
    if (a.isMiss()) return DValue(true);
    if (a.isNull()) return DValue(false);
    assert(a.isBool());
    return DValue(!a.getBool());
}

DValue operator + (const DValue& a, const DValue& b) {
    if (a.isMiss() || b.isMiss()) return DValue(true);
    if (a.isNull() || b.isNull()) return DValue(false);
    assert(a.isNum() && b.isNum());
    if (a.isInt() && b.isInt()) return DValue(a.getInt() + b.getInt());
    else return DValue(a.getReal() + b.getReal());
}

DValue operator - (const DValue& a, const DValue& b) {
    if (a.isMiss() || b.isMiss()) return DValue(true);
    if (a.isNull() || b.isNull()) return DValue(false);
    assert(a.isNum() && b.isNum());
    if (a.isInt() && b.isInt()) return DValue(a.getInt() - b.getInt());
    else return DValue(a.getReal() - b.getReal());
}

DValue operator * (const DValue& a, const DValue& b) {
    if (a.isMiss() || b.isMiss()) return DValue(true);
    if (a.isNull() || b.isNull()) return DValue(false);
    assert(a.isNum() && b.isNum());
    if (a.isInt() && b.isInt()) return DValue(a.getInt() * b.getInt());
    else return DValue(a.getReal() * b.getReal());
}

DValue operator / (const DValue& a, const DValue& b) {
    if (a.isMiss() || b.isMiss()) return DValue(true);
    if (a.isNull() || b.isNull()) return DValue(false);
    assert(a.isNum() && b.isNum());
    if (a.isInt() && b.isInt()) return DValue(a.getInt() / b.getInt());
    else return DValue(a.getReal() / b.getReal());
}

DValue operator % (const DValue& a, const DValue& b) {
    if (a.isMiss() || b.isMiss()) return DValue(true);
    if (a.isNull() || b.isNull()) return DValue(false);
    assert(a.isInt() && b.isInt());
    return DValue(a.getInt() % b.getInt());
}

bool DValueLT::operator() (const DValue& lhs, const DValue& rhs) const {
    if (lhs.isMiss() || rhs.isMiss()) return true;
    if (lhs.isNull() || rhs.isNull()) return false;
    if (lhs.isNum() && rhs.isNum()) {
        if (lhs.isInt() && rhs.isInt()) return lhs.getInt() < rhs.getInt();
        return lhs.getReal() < rhs.getReal();
    } else if (lhs.isBool() && rhs.isBool()) {
        return lhs.getBool() < rhs.getBool();
    } else if (lhs.isString() && rhs.isString()) {
        return lhs.getString() < rhs.getString();
    }
    return false;
}
