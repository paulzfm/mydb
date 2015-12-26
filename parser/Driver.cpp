#include "Driver.h"

bool Driver::parseString(const std::string &in)
{
    scan_begin(in.c_str());
    yy::SQLParser parser(*this);
    int ret = parser.parse();
    scan_end();

    return ret == 0;
}

bool Driver::parseFile(const std::string &path)
{
    return false;
}

void Driver::error(const yy::location& l, const std::string& m)
{
    std::cerr << l << ": " << m << std::endl;
}

void Driver::error(const std::string& m)
{
    std::cerr << m << std::endl;
}
