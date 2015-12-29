#ifndef UTIL_PRINT_H_
#define UTIL_PRINT_H_

#include <string>
#include <vector>

/* Print in MySQL style like this:
+------------+------------+...+
| heads[0]   | heads[1]   |...|
+------------+------------+...+
| rows[0][0] | rows[0][1] |...|
| rows[1][0] | rows[1][1] |...|
...
+------------+------------+...+
WARNING: may modify rows if some row's size < heads'size
    (fill in these cells with empty string)
*/
std::string tableToString(std::vector<std::string>& heads,
    std::vector< std::vector<std::string> >& rows);

#endif // UTIL_PRINT_H_
