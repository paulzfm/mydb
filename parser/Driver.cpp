#include "Driver.h"

bool Driver::parseString(const std::string &in)
{
    hasErr = false;

    scan_begin(in.c_str());
    yy::SQLParser parser(*this);
    int ret = parser.parse();
    scan_end();

    return (ret == 0 && !hasErr);
}

bool Driver::parseFile(const std::string &path)
{
    return false;
}

void Driver::error(const yy::location& l, const std::string& m)
{
    hasErr = true;
    err << "at " << l << ": " << m << ".\n";
}

void Driver::error(const std::string& m)
{
    hasErr = true;
    err << m << ".\n";
}
