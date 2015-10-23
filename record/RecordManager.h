// Record manager associated with a file (table).

#ifndef RECORD_RECORD_MANAGER_H_
#define RECORD_RECORD_MANAGER_H_

#include "type.h"

struct Object
{
    DType type; // data type
    char* data; // binary data
};

struct Record
{
    int rid; // record id
    std::map<int, Object> attr; // key-value pairs (cid, obj)
};

class RecordManager
{
public:
    RecordManager(const std::string& db, const std::string& table);

    // insert a record
    //   returns the id of the last inserted record
    int insert(const Record& rec);

    // remove a record
    //   returns false if no such record
    bool remove(int rid);

    // update a record
    //   returns false if no such record
    bool update(int rid, const Record& rec);

    // query a record
    Record& query(int rid) const;

    // query records
    std::vector<Record>& querySome(const std::vector<int>& rids) const;

    // the id of the last record
    int last() const;

    // total number of records
    int size() const;
};

#endif // RECORD_RECORD_MANAGER_H_