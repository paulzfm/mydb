#include "types.h"

void Byte::serialize(char *dst, int8_t val) const
{
    int8_t *ptr = &val;
    memcpy(dst, ptr, 1);
}

int8_t Byte::unserialize(char *buf) const
{
    int8_t *ptr = (int8_t*)buf;
    return *ptr;
}

void Short::serialize(char *dst, int16_t val) const
{
    int16_t *ptr = &val;
    memcpy(dst, ptr, 2);
}

int16_t Short::unserialize(char *buf) const
{
    int16_t *ptr = (int16_t*)buf;
    return *ptr;
}

void Int::serialize(char *dst, int32_t val) const
{
    int32_t *ptr = &val;
    memcpy(dst, ptr, 4);
}

int32_t Int::unserialize(char *buf) const
{
    int32_t *ptr = (int32_t*)buf;
    return *ptr;
}

void Long::serialize(char *dst, int64_t val) const
{
    int64_t *ptr = &val;
    memcpy(dst, ptr, 8);
}

int64_t Long::unserialize(char *buf) const
{
    int64_t *ptr = (int64_t*)buf;
    return *ptr;
}

void Float::serialize(char *dst, float val) const
{
    float *ptr = &val;
    memcpy(dst, ptr, 4);
}

float Float::unserialize(char *buf) const
{
    float *ptr = (float*)buf;
    return *ptr;
}

void Double::serialize(char *dst, double val) const
{
    double *ptr = &val;
    memcpy(dst, ptr, 8);
}

double Double::unserialize(char *buf) const
{
    double *ptr = (double*)buf;
    return *ptr;
}

void String::serialize(char *dst, const std::string& val) const
{
    strcpy(dst, val.c_str());
}

std::string String::unserialize(char *buf) const
{
    return std::string(buf);
}
