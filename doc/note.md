# Database design

## 1. Internal Storage

### a. 数据表

每页8KB，记录按32bytes对齐。每32bytes记为一个word。

第1页各种metadata：每条记录长度、记录个数、页是否空闲等。

* +0    int         record_length (in bytes)
* +4    int         record_num
* +8    int         last_rid (record id)
* +12   int         last_pid (page id)
* ...
* +1K   byte[7K]    freemap

注意：freemap用`1`表示**空闲**，`0`表示**占用**。由于第1页已被占用，`freemap[0] = 0`。

以后各页：前32字节为bitmap，表示各word空闲情况，`freemap[0] = 0`； 以后为各条记录。

* +0    byte[32]    freemap
* +32   ...         records

每条record的前4字节为rid，以后为数据。

* +0    int         rid
* +4    ...         data

写入记录只关心将字符流（`char*`)写入相应位置，不关心内容。
修改时先查询并读取相应记录，修改后写入。
查询时上层传入函数`bool match(char*)，`，将其作用于每条记录并判断是否符合要求。

### b. 系统表

支持类型：BOOL, TINYINT, SMALLINT, INT, BIGINT, FLOAT, DOUBLE, CHAR, VARCHAR, TEXT*, ENUM*, SET*
支持约束：NOT NULL, UNIQUE, PRIMARY KEY, DEFAULT, FOREIGN KEY*, CHECK*
*:计划支持

系统表不做序列化，直接将struct的binary representation存入文件中。每个用户表的字段和约束单独存放在独立的文件中。

#### a) 字段

* int       cid
* char[16]  name
* int       type

24 bytes

#### b) 约束

* int       cid
* int       type
* int64_t   data

16 bytes

### c. 内存中的数据库对象（记录）

    Record:
        int rid;
        map<int, Object> attr;

Record中的属性顺序按字段定义（即系统表）顺序存放。

    Object:
        DType type;
        char* data;

type表示该Object的类型，data即其实际数据。
