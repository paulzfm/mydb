#include "values.h"

DValue::DValue() : type(0) {
    data = NULL;
}

DValue::DValue(bool val) : type(DType::BYTE) {
    data = new char;
    *data = val ? 1 : 0;
}

DValue::DValue(int64_t val) : type(DType::LONG) {
    data = new char[8];
    memcpy(data, &val, 8);
}

DValue::DValue(double val) : type(DType::DOUBLE) {
    data = new char[8];
    memcpy(data, &val, 8);
}

DValue::DValue(string& val) : type(DType::STRING, len) {
    int len = val.length();
    data = new char[len + 1];
    memcpy(data, val.c_str(), len);
    data[len] = '\0';
}

DValue::~DValue() {
    delete [] data;
}

bool DValue::isNull() const {
    return data == NULL;
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
    double val;
    memcpy(&val, data, 8);
    return val;
}

string DValue::getString() const {
    return string(data);
}

DValue operator == (const DValue& a, const DValue& b) {
    assert(a.type.ident == b.type.ident);
    if (a.isBool()) return DValue(a.getBool() == b.getBool());
    if (a.isInt()) return DValue(a.getInt() == b.getInt());
    if (a.isReal()) return DValue(a.getReal() == b.getReal());
    if (a.isString()) return DValue(a.getString() == b.getString());
    return DValue(false);
}

DValue operator != (const DValue& a, const DValue& b) {
    assert(a.isBool() && b.isBool());
    if (a.isInt() && b.isInt()) return DValue(a.getInt() != b.getInt());
    else return DValue(a.getReal() != b.getReal());
}

DValue operator < (const DValue& a, const DValue& b) {
    assert(a.isNum() && b.isNum());
    if (a.isInt() && b.isInt()) return DValue(a.getInt() < b.getInt());
    else return DValue(a.getReal() < b.getReal());
}

DValue operator > (const DValue& a, const DValue& b) {
    assert(a.isNum() && b.isNum());
    if (a.isInt() && b.isInt()) return DValue(a.getInt() > b.getInt());
    else return DValue(a.getReal() > b.getReal());
}

DValue operator <= (const DValue& a, const DValue& b) {
    assert(a.isNum() && b.isNum());
    if (a.isInt() && b.isInt()) return DValue(a.getInt() <= b.getInt());
    else return DValue(a.getReal() <= b.getReal());
}

DValue operator >= (const DValue& a, const DValue& b) {
    assert(a.isNum() && b.isNum());
    if (a.isInt() && b.isInt()) return DValue(a.getInt() >= b.getInt());
    else return DValue(a.getReal() >= b.getReal());
}

DValue operator && (const DValue& a, const DValue& b) {
    assert(a.isBool() && b.isBool());
    return DValue(a.getBool() && b.getBool());
}

DValue operator || (const DValue& a, const DValue& b) {
    assert(a.isBool() && b.isBool());
    return DValue(a.getBool() || b.getBool());
}

DValue operator ! (const DValue& a) {
    assert(a.isBool());
    return DValue(!a.getBool());
}

DValue operator + (const DValue& a, const DValue& b) {
    assert(a.isNum() && b.isNum());
    if (a.isInt() && b.isInt()) return DValue(a.getInt() + b.getInt());
    else return DValue(a.getReal() + b.getReal());
}

DValue operator - (const DValue& a, const DValue& b) {
    assert(a.isNum() && b.isNum());
    if (a.isInt() && b.isInt()) return DValue(a.getInt() - b.getInt());
    else return DValue(a.getReal() - b.getReal());
}

DValue operator * (const DValue& a, const DValue& b) {
    assert(a.isNum() && b.isNum());
    if (a.isInt() && b.isInt()) return DValue(a.getInt() * b.getInt());
    else return DValue(a.getReal() * b.getReal());
}

DValue operator / (const DValue& a, const DValue& b) {
    assert(a.isNum() && b.isNum());
    if (a.isInt() && b.isInt()) return DValue(a.getInt() / b.getInt());
    else return DValue(a.getReal() / b.getReal());
}

DValue operator % (const DValue& a, const DValue& b) {
    assert(a.isInt() && b.isInt());
    return DValue(a.getInt() % b.getInt());
}

