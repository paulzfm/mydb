# mydb

A simple one-user single-thread database as a final project.

### Prerequisites

- `flex` >= 2.5.31
- `bison` >= 3.0.4

### Install

On unix-like systems, type

    $ make

### Usage

    ./mydb [-s port | -r addr -p port | -f file | -h]

Options:

	-s    start as server
	-r    connect remote server (ip)
	-p    connect remote server (port)
	-f    specify input SQL file
    -h    see help

To launch interactive terminal, run without any options.

#### Interactive Environment

Type

    ./mydb

You will see the following welcome information

    Welcome to mydb!

    Powered by Wen He and Zhu Fengmin.

    Type SQL statements end with ';' or type 'exit' or 'quit' to quit.
    Run './mydb -h' to see more options.

    mydb>

Type SQL statements to execute queries

    mydb> show databases;

Or type `exit` or `quit` to quit `mydb`

    mydb> exit

#### Specify an Input File

Type

    ./mydb -f <your_input_file_name>

then you can see the results.

#### Remote Connection

First start a server

    ./mydb -s <port>

Then connect the server

    ./mydb -r <ip> -p <port>

And you can now using it in interactive environment.

### SQL Support

See [SQL Support Document](https://github.com/paulzfm/mydb/blob/master/doc/SQL%20Support%20Document.pdf) for more information.

### Report

See [Final Report](https://github.com/paulzfm/mydb/blob/master/doc/final-report.pdf). Notice that it is written in Chinese.
