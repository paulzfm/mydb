#include "RecordManager.h"

RecordManager::RecordManager(const std::string& path)
{
    fm = new FileManager();
    bpm = new BufPageManager(fm);
    fm->openFile(path, fid);
}

RecordManager::~RecordManager()
{
    delete fm;
    delete bpm;
}

int RecordManager::insert(const Record& rec)
{
    
}