#ifndef UTIL_PRINT_H_
#define UTIL_PRINT_H_

#include <string>
#include <vector>

// TODO: implement this
/* Print in MySQL style like this:
+------------+------------+...+
| heads[0]   | heads[1]   |...|
+------------+------------+...+
| rows[0][0] | rows[0][1] |...|
| rows[1][0] | rows[1][1] |...|
...
+------------+------------+...+
*/
std::string tableToString(const std::vector<std::string>& heads,
    const std::vector< std::vector<std::string> >& rows)
{
    return "";
}

#endif
