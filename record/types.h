// Data types for mydb.
// type DType = Short | Int | Long | Float | Double | Bool | Char | String <bytes>

#ifndef RECORD_TYPES_H_
#define RECORD_TYPES_H_

#include <string>

class DType
{
public:
    DType(int ident) : ident(ident)
    {
        ident = ident;
        switch (ident) {
            case BYTE:      bytes = 1; break;
            case SHORT:     bytes = 2; break;
            case INT:       bytes = 4; break;
            case LONG:      bytes = 8; break;
            case FLOAT:     bytes = 4; break;
            case DOUBLE:    bytes = 8; break;
            case CHAR:      bytes = 1; break;
            case STRING:    bytes = 256; break;
            default: ; // error
        }
    }

    DType(int ident, int length)
    {
        if (ident != STRING) ; // error
        ident = STRING;
        bytes = length;
    }

    // identifier: for pattern matching
    int ident;
    // We support the following types:
    const static int BYTE = 100;
    const static int SHORT = 101;
    const static int INT = 102;
    const static int LONG = 103;
    const static int FLOAT = 104;
    const static int DOUBLE = 105;
    const static int CHAR = 106;
    const static int STRING = 107;

    // length of current type in bytes
    int bytes;
};

class Byte : public DType
{
public:
    Byte() : DType(DType::BYTE) {}

    void serialize(char *dst, int8_t val) const;
    int8_t unserialize(char *buf) const;
};

class Short : public DType
{
public:
    Short() : DType(DType::SHORT) {}

    void serialize(char *dst, int16_t val) const;
    int16_t unserialize(char *buf) const;
};

class Int : public DType
{
public:
    Int() : DType(DType::INT) {}

    void serialize(char *dst, int32_t val) const;
    int32_t unserialize(char *buf) const;
};

class Long : public DType
{
public:
    Long() : DType(DType::LONG) {}

    void serialize(char *dst, int64_t val) const;
    int64_t unserialize(char *buf) const;
};

class Float : public DType
{
public:
    Float() : DType(DType::FLOAT) {}

    void serialize(char *dst, float val) const;
    float unserialize(char *buf) const;
};

class Double : public DType
{
public:
    Double() : DType(DType::DOUBLE) {}

    void serialize(char *dst, double val) const;
    double unserialize(char *buf) const;
};

class Char : public DType
{
    Char() : DType(DType::CHAR) {}

    void serialize(char *dst, char val) const;
    char unserialize(char *buf) const;
};

class String : public DType
{
public:
    String(int length) : DType(DType::STRING, length) {}

    void serialize(char *dst, const std::string& val) const;
    std::string unserialize(char *buf) const;
};

#endif // RECORD_TYPES_H_