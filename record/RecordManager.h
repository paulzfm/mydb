#ifndef RECORD_RECORD_MANAGER_H_
#define RECORD_RECORD_MANAGER_H_

#include "type.h"
#include "../lib/fs/fileio/FileManager.h"
#include "../lib/fs/bufmanager/BufPageManager.h"

#include <string>
#include <map>
#include <vector>

// Object (a cell in table)
struct Object
{
    DType type; // data type
    char* data; // binary data
};

// Record (a row in table)
struct Record
{
    int rid; // record id
    std::map<int, Object> attr; // key-value pairs (cid, obj)
};

// Record manager associated with a file (table).
class RecordManager
{
public:
    RecordManager(const std::string& path);

    ~RecordManager();

    // insert a record
    //   returns the id of the last inserted record
    int insert(const Record& rec);

    // remove a record
    //   returns false if no such record
    bool remove(int rid);

    // replace a record
    //   returns false if no such record
    bool replace(int rid, const Record& rec);

    // load some records
    std::vector<Record>& load(const std::vector<int>& rids) const;

    // load a record
    Record& loadOne(int rid) const;

    // load all records
    std::vector<Record>& loadAll() const;

    // the id of the last record
    int last() const;

    // total number of records
    int size() const;

private:
    FileManager *fm;
    BufPageManager *bpm;
    int fid; // file id
};

#endif // RECORD_RECORD_MANAGER_H_