#ifndef QUERY_UTILS_H_
#define QUERY_UTILS_H_

#include "../util/common.h"
#include "../sys/Table.h"
#include "../parser/Tree.h"
#include "../record/RecordManager.h"
#include "Evaluator.h"
#include <unordered_map>

using std::unordered_map;

std::function<bool(const Record&)> getFilter(Table* table, BoolExpr* expr);
std::function<bool(const unordered_map<string, DValue>&)> getJoinFilter(BoolExpr* expr);
BoolExpr* getBoolExpr(const string& tb, const string& col, DValue val);

#endif
