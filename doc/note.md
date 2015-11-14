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


系统表不做序列化，直接将struct的binary representation存入文件中。

系统数据库列表为dblist.dat，存放在根目录下。
第一行为n，为database的数量。
接下来每行一个字符串，为数据库名。


数据库的表列表文件为tablelist.dat，存放在数据库的目录下。
每部分起始为表描述，接下来若干个字段描述，最后若干个约束描述。
每个表最多支持255个字段和255个约束。

#### a) 表

* char[16]  name
* char      column_num;
* char      constraint_num;

18 bytes

#### b) 字段

* int       cid
* char[16]  name
* int       type

24 bytes

#### c) 约束

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


## 2. 目录结构

---- /
   |---- EXECUTABLE ELF 
   |---- mydb.conf      // system configuration file
   |---- dblist.dat     // list of all existing databases
   |---- databases
   |   |---- DBNAME1
   |   |   |---- tablelist.dat      // list of all tables in DBNAME1
   |   |   |---- TABLENAME1.dat     // data stored in TABLENAME1
   |   |   |---- TABLENAME1.idx     // indexes of TABLENAME1
   |   |   |---- TABLENAME2.dat
   |   |   |---- TABLENAME2.idx
   |   |   |---- ...
   |   |
   |   |---- DBNAME2
   |   |   |---- tablelist.dat
   |   |   |---- TABLENAME1.dat
   |   |   |---- TABLENAME2.dat
   |   |   |---- ...

