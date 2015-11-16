#include "RecordManager.h"

#include <stdio.h>
#include <vector>

#define FILE_PATH "testdb.db"
#define RECORD_LEN 16

void printRecord(const Record& rec)
{
    printf("{\n");
    printf("  rid: %d\n", rec.rid);
    int *ptr = (int*)rec.data;
    printf("  data: [%d %d %d %d]\n", ptr[0], ptr[1], ptr[2], ptr[3]);
}

void test()
{
    // create a new table
    RecordManager rman(FILE_PATH, RECORD_LEN);

    // write some record
    int buf[4];
    for (int i = 0; i < 5; i++) {
        buf[0] = buf[1] = buf[2] = buf[3] = i;
        rman.insert((char*)buf);
        printf("After inserting %d, last = %d, count = %d", i, rman.last(), rman.count());
    }

    // load all records
    std::vector<Record> records;
    rman.loadAll(records);
    for (const auto& rec : records) {
        printRecord(rec);
    }

    // replace the last record
}

int main()
{
    test();

    return 0;
}
