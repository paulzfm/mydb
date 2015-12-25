#include "Driver.h"

void Driver::parse(const std::string &in)
{
    file = in;
    scan_begin();
    yy::SQLParser parser(*this);
    parser.parse();
    scan_end();
}

void Driver::error(const yy::location& l, const std::string& m)
{
    std::cerr << l << ": " << m << std::endl;
}

void Driver::error(const std::string& m)
{
    std::cerr << m << std::endl;
}
