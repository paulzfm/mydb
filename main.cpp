#include "engine/Engine.h"
#include "engine/sock.h"

#include <iostream>
#include <fstream>
#include <string>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

bool readFile(const std::string& file, std::string& ret)
{
    std::ifstream ifs(file);
    if (!ifs)
    {
        std::cerr << "[FATAL] unable to open file '" << file << "'.\n";
        return false;
    }

    std::string content((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
    ret = content;
    ifs.close();

    return true;
}

int main(int argc, char **argv)
{
    bool fflag = false; // -f file
    char *file = NULL;
    bool rflag = false; // -r addr -p port
    char *ip = new char[40];
    strcpy(ip, "127.0.0.1");
    char *port = new char[40];
    strcpy(port, "12345");
    bool sflag = false; // -s local_port
    char *sport = new char[40];
    strcpy(sport, "12345");

    opterr = 0;
    char c;
    while ((c = getopt(argc, argv, "s:f:r:p:h")) != -1) {
    switch (c) {
        case 'h':
            fprintf(stderr, "Usage: %s [-s port | -r addr -p port | -f file | -h]\n", argv[0]);
            fprintf(stderr, "Options:\n");
            fprintf(stderr, "\t-s    start as server\n");
            fprintf(stderr, "\t-r    connect remote server (ip)\n");
            fprintf(stderr, "\t-p    connect remote server (port)\n");
            fprintf(stderr, "\t-f    specify input SQL file\n");
            fprintf(stderr, "To launch interactive terminal, run without any options.\n");
            exit(1);
        case 's':
            sflag = true;
            sport = optarg;
            break;
        case 'f':
            fflag = true;
            file = optarg;
            break;
        case 'r':
            rflag = true;
            ip = optarg;
            break;
        case 'p':
            rflag = true;
            port = optarg;
            break;
        case '?':
            if (optopt == 'f' || optopt == 'r' || optopt == 's') {
                fprintf(stderr, "Option -%c requires an argument.\n", optopt);
            } else if(isprint (optopt)) {
                fprintf(stderr, "Unknown option `-%c'.\n", optopt);
            } else {
                fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
            }
            exit(1);
        default:
            exit(1);
      }
    }

    MyBitMap::initConst();

    if (sflag) { // start server
        runServer(atoi(sport));
        return 0;
    } else if (rflag) { // start client
        int sock = connServer(ip, atoi(port));
        if (sock < 0) {
            return 1;
        }
        sendCMD(sock);
        return 0;
    } else { // local
        if (fflag) { // from file
            std::string input;
            std::string path(file);
            if (readFile(path, input)) {
                StringEngine se(input);
                se.setFile(path);
                se.run();
                return 0;
            }
        } else { // interactive
            InteractiveEngine ie;
            ie.run();
            return 0;
        }
    }

    return 0;
}
