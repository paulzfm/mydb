#include "RecordManager.h"

#include <stdio.h>
#include <assert.h>
#include <vector>

#define FILE_PATH "testdb.db"
#define RECORD_LEN 16

void printRecord(const Record& rec)
{
    printf("{\n");
    printf("  rid: %d\n", rec.rid);
    int *ptr = (int*)rec.data;
    printf("  data: [%d %d %d %d]\n", ptr[0], ptr[1], ptr[2], ptr[3]);
    printf("  page: %d, offset: %d\n", rec.page, rec.offset);
    printf("}\n");
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
        printf("After inserting %d, last = %d, count = %d\n", i, rman.last(), rman.count());
    }

    // load all records
    std::vector<Record> records;
    rman.loadAll(records);
    printf("%lu records found.\n", records.size());
    for (const auto& rec : records) {
        printRecord(rec);
    }

    // remove the 3rd record
    rman.remove(1, 3);

    // replace the last record
    int newBuf[] = {10, 10, 10, 10};
    rman.replace(1, 5, (char*)newBuf);

    // insert a new record
    buf[0] = buf[1] = buf[2] = buf[3] = 5;
    rman.insert((char*)buf);
    printf("After inserting %d, last = %d, count = %d\n", 5, rman.last(), rman.count());

    // load all records
    records.clear();
    rman.loadAll(records);
    printf("%lu records found.\n", records.size());
    for (const auto& rec : records) {
        printRecord(rec);
    }

    // query record whose data % 2 == 0
    auto filter = [](const Record& rec) {
        int *ptr = (int*)rec.data;
        return ptr[0] % 2 != 0;
    };
    records.clear();
    rman.query(filter, records);
    printf("%lu records found.\n", records.size());
    for (const auto& rec : records) {
        printRecord(rec);
    }

    // load the first found
    printf("load 1st found record\n");
    Record rec;
    rman.load(records[0].page, records[0].offset, rec);
    printRecord(rec);
}

void test_read()
{
    // open a table
    RecordManager rman(FILE_PATH);

    // load all records
    std::vector<Record> records;
    rman.loadAll(records);
    printf("%lu records found.\n", records.size());
    for (const auto& rec : records) {
        printRecord(rec);
    }
}

int main()
{
    test();
    // test_read();

    return 0;
}
