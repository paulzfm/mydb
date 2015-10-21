# Database design

## 1. Internal Storage

### a. 数据表

每页8KB，记录按32字节对齐。

第一页各种metadata：每条记录长度、记录个数、空闲页计数等。其中freemap存放该页还能放几个record:

* +0    int         record_length (in bytes)
* +4    int         record_num
* ...
* +4K   char[4096]  freemap

以后各页：前32字节为bitmap，表示空闲情况，第1bit固定为1； 以后为各条记录。

* +0    char[32]    freemap
* +32B  ...         records

写入记录只关心将字符流（`char*`)写入相应位置，不关心内容。
修改时先查询并读取相应记录，修改后写入。
查询时上层传入函数`bool match(char*)，`，将其作用于每条记录并判断是否符合要求。

### b. 系统表

支持类型：BOOL, TINYINT, SMALLINT, INT, BIGINT, FLOAT, DOUBLE, CHAR, VARCHAR, TEXT*, ENUM*, SET*
支持约束：NOT NULL, UNIQUE, PRIMARY KEY, DEFAULT, FOREIGN KEY*, CHECK*
*:计划支持

系统表不做序列化，直接将struct的binary representation存入文件中。

#### a) 字段表

* int       cid
* char[16]  name
* int       type

24 bytes

#### b) 约束表

* int       cid
* int       type
* int       data