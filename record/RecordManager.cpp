#include "RecordManager.h"
#include "../util/BitMap.h"

#include <math.h>

RecordManager::RecordManager(const std::string& path)
{
    // create fs manager
    fm = new FileManager();
    bpm = new BufPageManager(fm);
    fm->openFile((path + ".dat").c_str(), fid);

    // load meta data from first page
    int index;
    int *b = (int*)(bpm->getPage(fid, 0, index));
    length = b[0];
    words = ceil(length / 32.0);
    size = b[1];
    rid = b[2];
    pid = b[3];
}

RecordManager::RecordManager(const std::string& path, int length)
    : length(length)
{
    // set meta data
    words = ceil(length / 32.0);
    size = 0;
    rid = 0;
    pid = 0;

    // create fs manager
    fm = new FileManager();
    bpm = new BufPageManager(fm);
    fm->createFile((path + ".dat").c_str());
    fm->openFile((path + ".dat").c_str(), fid);

    // initialize first page
    int index;
    int *b = (int*)(bpm->allocPage(fid, pid, index, false));
    b[0] = length;
    b[1] = size;
    b[2] = rid;
    b[3] = pid;
    for (int i = RM_FREE_MAP_OFFSET; i < (RM_FREE_MAP_OFFSET + RM_FREE_MAP_MAX_SIZE); i++) { // all offsets: 1
        b[i] = -1; // 1111...1111
    }
    BitMap::reset(b + RM_FREE_MAP_OFFSET, 0); // freemap[0] = 0
    bpm->markDirty(index);
}

RecordManager::~RecordManager()
{
    bpm->close();
    fm->closeFile(fid);
    delete fm;
    delete bpm;
}

std::pair<int, int> RecordManager::insert(char *data)
{
    // find empty page
    int index0;
    int *b0 = (int*)(bpm->getPage(fid, 0, index0));
    int pageId = BitMap::findOne(b0 + RM_FREE_MAP_OFFSET, pid + 1);
    if (pageId == -1) { // no empty page: create a new one
        pid++;
        pageId = pid;
        int index;
        int *b = (int*)(bpm->allocPage(fid, pageId, index, false));
        for (int i = 0; i < RM_PAGE_FREE_MAP_SIZE; i++) { // all offsets: 1
            b[i] = -1; // 1111...1111
        }
        BitMap::reset(b, 0); // freemap[0] = 0
        bpm->markDirty(index);

        b0[3] = pid;
        bpm->markDirty(index0);
    }

    // find empty offset
    int index1;
    int *b1 = (int*)(bpm->getPage(fid, pageId, index1));
    int offset = BitMap::findOne(b1, RM_PAGE_FREE_MAP_SIZE << 5);
    for (int i = 0; i < words; ++i)
        BitMap::reset(b1, offset + i);

    // write record
    rid++;
    size++;
    b1[offset * RM_WORD_SIZE] = rid;
    memcpy(b1 + offset * RM_WORD_SIZE + 1, data, length);
    bpm->markDirty(index1);
    if (BitMap::findOne(b1, RM_PAGE_FREE_MAP_SIZE << 5) == -1) { // current page is full
        BitMap::reset(b0 + RM_FREE_MAP_OFFSET, pageId);
    }

    // update meta data in first page
    b0[1] = size;
    b0[2] = rid;
    bpm->markDirty(index0);

    return make_pair(pageId, offset);
}

bool RecordManager::remove(int page, int offset)
{
    int index1;
    int *b1 = (int*)(bpm->getPage(fid, page, index1));
    if (BitMap::test(b1, offset)) { // free space cannot be removed
        return false;
    }

    // update meta data in current page
    BitMap::set(b1, offset);
    bpm->markDirty(index1);

    // update meta data in first page
    int index0;
    int *b0 = (int*)(bpm->getPage(fid, 0, index0));
    BitMap::set(b0 + RM_FREE_MAP_OFFSET, page);
    size--;
    b0[1] = size;
    bpm->markDirty(index0);

    return true;
}

bool RecordManager::replace(int page, int offset, char *data)
{
    int index;
    int *b = (int*)(bpm->getPage(fid, page, index));
    if (BitMap::test(b, offset)) { // free space cannot be replaced
        return false;
    }

    // update data
    memcpy(b + offset * RM_WORD_SIZE + 1, data, length);
    bpm->markDirty(index);
    return true;
}

bool RecordManager::load(int page, int offset, Record& rec)
{
    int index;
    int *b = (int*)(bpm->getPage(fid, page, index));
    if (BitMap::test(b, offset)) { // free space cannot be loaded
        return false;
    }

    // read data
    int *start = b + offset * RM_WORD_SIZE;
    rec.set(*start, start + 1, length, page, offset);
    // rec = Record(*start, start + 1, length, page, offset);
    bpm->access(index);
    return true;
}

void RecordManager::loadAll(std::vector<Record>& records)
{
    for (int page = 1; page <= pid; page++) {
        int index;
        int *b = (int*)(bpm->getPage(fid, page, index));
        std::vector<int> offsets;
        BitMap::findZeros(b, RM_PAGE_FREE_MAP_SIZE << 5, offsets);
        offsets.erase(offsets.begin());
        for (int offset: offsets) {
            if ((offset - 1) % words == 0) {
                int *start = b + offset * RM_WORD_SIZE;
                records.push_back(Record(*start, start + 1, length, page, offset));
            }
        }
        bpm->access(index);
    }
}

void RecordManager::query(const std::function<bool(const Record&)>& filter, std::vector<Record>& records)
{
    for (int page = 1; page <= pid; page++) {
        int index;
        int *b = (int*)(bpm->getPage(fid, page, index));
        std::vector<int> offsets;
        BitMap::findZeros(b, RM_PAGE_FREE_MAP_SIZE << 5, offsets);
        offsets.erase(offsets.begin());
        for (int offset: offsets) {
            if ((offset - 1) % words == 0) {
                int *start = b + offset * RM_WORD_SIZE;
                Record rec(*start, start + 1, length, page, offset);
                if (filter(rec)) {
                    records.push_back(rec);
                }
            }
        }
    }
}

bool RecordManager::queryOne(const std::function<bool(const Record&)>& filter, Record& record)
{
    for (int page = 1; page <= pid; page++) {
        int index;
        int *b = (int*)(bpm->getPage(fid, page, index));
        std::vector<int> offsets;
        BitMap::findZeros(b, RM_PAGE_FREE_MAP_SIZE << 5, offsets);
        offsets.erase(offsets.begin());
        for (int offset: offsets) {
            if ((offset - 1) % words == 0) {
                int *start = b + offset * RM_WORD_SIZE;
                Record rec(*start, start + 1, length, page, offset);
                if (filter(rec)) {
                    record = rec;
                    return true;
                }
            }
        }
    }
    return false;
}

int RecordManager::last() const
{
    return rid;
}

int RecordManager::count() const
{
    return size;
}

int RecordManager::width() const
{
    return length;
}

void RecordManager::getIndexes(vector<string>& vec) const {
    for (const auto& index : indexes)
        vec.push_back(index.first);
}

void RecordManager::createIndex(const string& col) {
    indexes[col];
}

void RecordManager::dropIndex(const string& col) {
    indexes.erase(col);
}

void RecordManager::addIndex(const string& col, const DValue& val, pair<int, int> pos) {
    auto& index = indexes[col];
    index.insert(make_pair(val, pos));
}

void RecordManager::removeIndex(const string& col, const DValue& val, pair<int, int> pos) {
    auto& index = indexes[col];
    for (auto iter = index.find(val); iter != index.end() && (iter->first == val).getBool(); ++iter) {
        if (iter->second == pos) {
            index.erase(iter);
            return;
        }
    }
}
