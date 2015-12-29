#include "sock.h"
#include "Engine.h"

#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <dirent.h>
#include <string.h>
#include <arpa/inet.h>

#define BUF_SIZE 2048

typedef struct {
    int sock;
} serve_args;

void *serve(void* args)
{
    serve_args* pargs = (serve_args*)args;
    int sock = pargs->sock;
    printf("Service started for sock %d\n", sock);

    ServerEngine se;

    char cmd[BUF_SIZE];
    while (true) {
        memset(cmd, 0x00, sizeof(cmd));
        int ret = recv(sock, cmd, sizeof(cmd), 0);
        if (ret == -1) {    // error
            fprintf(stderr, "***Error: failed to receive command (errno: %d)\n", errno);
            break;
        } else if (ret == 0) {  // connection close
            break;
        } else {    // deal with commands
            if (strcmp(cmd, "quit") == 0) {
                printf("Server stopped for sock %d\n", sock);
                break;
            } else {
                std::string input(cmd);
                std::string result = se.execute(input);
                send(sock, result.c_str(), result.length(), 0);
            }
        }
    }
}

void runServer(int port)
{
    // create socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock < 0) {
        fprintf(stderr, "***Error: failed to initialize sock\n");
        return;
    }

    // bind
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = port;

    if (bind(sock, (sockaddr*)&addr, sizeof(addr)) < 0) {
        fprintf(stderr, "***Error: failed to bind (errno: %d)\n", errno);
        return;
    }

    // listen
    if (listen(sock, 2) < 0) {
        fprintf(stderr, "***Error: failed to listen (errno: %d)\n", errno);
        return;
    }

    printf("Server started at port %d\n", port);

    while (true) {
        int client_sock = accept(sock, (sockaddr*)0, (unsigned int*)sizeof(sockaddr_in));

        if (client_sock == -1) {
            fprintf(stderr, "***Error: accept failed (errno: %d)\n", errno);
            break;
        }

        // create a thread to deal with this client
        pthread_t pid;
        serve_args args;
        args.sock = client_sock;
        if (pthread_create(&pid, NULL, serve, (void *)&args) != 0) {
            fprintf(stderr, "***Error: thread create failed\n");
            break;
        }
    }

    close(sock);
}

int connServer(const char *ip, int port)
{
    // create socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock < 0) {
        fprintf(stderr, "***Error: failed to initialize socket\n");
        return -1;
    }

    // local address
    sockaddr_in data_addr;
    data_addr.sin_family = AF_INET;
    data_addr.sin_port = port;

    if (inet_aton(ip, &data_addr.sin_addr) == 0) {
        fprintf(stderr, "***Error: invalid ip addr\n");
        return -1;
    }

    printf("Connecting %s:%d\n", ip, port);

    // connect
    if (connect(sock, (sockaddr*)&data_addr, sizeof(data_addr)) == -1) {
        fprintf(stderr, "***Error: failed to connect server\n");
        return -1;
    }

    printf("OK.\n");

    return sock;
}

void sendCMD(int sock)
{
    char data[BUF_SIZE];

    std::cout << "Welcome to mydb!\n";
    std::string buf, buf1;
    while (true) {
        std::cout << "mydb> ";
        std::getline(std::cin, buf);
        buf = trim(buf);
        if (buf == "quit" || buf == "exit") {
            buf = "quit";
            send(sock, buf.c_str(), buf.length(), 0);
            std::cout << "Bye!\n";
            return;
        } else if (buf.back() != ';' || buf.length() == 0) { // non terminate
            while (true) {
                std::cout << "   -> ";
                std::getline(std::cin, buf1);
                buf1 = trim(buf1);
                buf += " ";
                buf += buf1;
                if (buf1.length() > 0 && buf1.back() == ';') { // terminate
                    break;
                }
            }
        }

        send(sock, buf.c_str(), buf.length(), 0);

        memset(data, 0x00, sizeof(data));
        recv(sock, data, sizeof(data), 0);
        std::cout << data;
    }
}
