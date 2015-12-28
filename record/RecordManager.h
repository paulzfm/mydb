#ifndef RECORD_RECORD_MANAGER_H_
#define RECORD_RECORD_MANAGER_H_

#include "../lib/fs/fileio/FileManager.h"
#include "../lib/fs/bufmanager/BufPageManager.h"

#include <string.h>
#include <string>
#include <vector>
#include <functional>

// Record (a row in table)
struct Record
{
    int rid; // record id
    int len; // data length
    char *data; // binary data

    int page;   // useful when calling query
    int offset; // useful when calling query

    Record() {}

    Record(int rid, int *buf, int length) : rid(rid), len(length)
    {
        data = new char[len];
        memcpy(data, buf, len);
    }

    Record(int rid, int *buf, int length, int pid, int offset)
        : rid(rid), len(length), page(pid), offset(offset)
    {
        data = new char[len];
        memcpy(data, buf, len);
    }

    Record(const Record& rec)
        : rid(rec.rid), len(rec.len), page(rec.page), offset(rec.offset)
    {
        data = new char[len];
        memcpy(data, rec.data, len);
    }

    void set(int rid, int *buf, int length, int pid, int offset)
    {
        this->rid = rid;
        this->len = length;
        this->page = pid;
        this->offset = offset;
        if (data) {
            delete[] data;
        }
        data = new char[length];
        memcpy(data, buf, length);
    }

    ~Record()
    {
        delete[] data;
    }
};

// NOTE: the number is measured in every 4 bytes, say, an integer
#define RM_FREE_MAP_OFFSET 256
#define RM_FREE_MAP_MAX_SIZE 1792
#define RM_PAGE_FREE_MAP_SIZE 8
#define RM_WORD_SIZE 8

// Record manager associated with a file (table).
class RecordManager
{
public:
    // open an existing file
    RecordManager(const std::string& path);

    // create a new file
    RecordManager(const std::string& path, int length);

    ~RecordManager();

    // insert a record
    //   returns false if the table if full
    bool insert(char *data);

    // remove a record
    //   returns false if no such record
    bool remove(int page, int offset);

    // replace a record
    //   returns false if no such record
    bool replace(int page, int offset, char *data);

    // load a record
    //   returns false if no such record
    bool load(int page, int offset, Record& rec);

    // load all records
    void loadAll(std::vector<Record>& records);

    // query records satisfing the filter
    //    filter :: const Record & -> bool
    void query(const std::function<bool(const Record&)>& filter, std::vector<Record>& records);

    // the id of the last record
    int last() const;

    // total number of records
    int count() const;

    // record length in bytes
    int width() const;

private:
    FileManager *fm;
    BufPageManager *bpm;
    int fid; // file id

    int length; // record length in types
    int words;  // record length in words
    int size;   // total number of records
    int rid;    // last rid
    int pid;    // last page id
};

#endif // RECORD_RECORD_MANAGER_H_
