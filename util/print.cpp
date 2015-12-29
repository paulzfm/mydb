#include "print.h"

std::string tableToString(std::vector<std::string>& heads,
    std::vector< std::vector<std::string> >& rows)
{
    int ncol = heads.size();
    for (auto& row : rows) {
        int sz = row.size();
        for (int i = 0; i < ncol - sz; i++) {
            row.push_back("");
        }
    }

    std::vector<int> max;
    for (int i = 0; i < ncol; i++) {
        int len = heads[i].length();
        for (const auto& row : rows) {
            if (row[i].length() > len) {
                len = row[i].length();
            }
        }
        max.push_back(len);
    }

    std::string line = "+";
    for (const auto& len : max) {
        for (int i = 0; i < len + 2; i++) {
            line += "-";
        }
        line += "+";
    }
    line += "\n";

    std::string ret = line;
    ret += "|";
    for (int i = 0; i < ncol; i++) {
        ret += " ";
        ret += heads[i];
        for (int j = 0; j <= max[i] - heads[i].length(); j++) {
            ret += " ";
        }
        ret += "|";
    }
    ret += "\n";
    ret += line;
    for (const auto& row : rows) {
        ret += "|";
        for (int i = 0; i < ncol; i++) {
            ret += " ";
            ret += row[i];
            for (int j = 0; j <= max[i] - row[i].length(); j++) {
                ret += " ";
            }
            ret += "|";
        }
        ret += "\n";
    }
    ret += line;

    return ret;
}
