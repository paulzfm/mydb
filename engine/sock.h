#ifndef ENGINE_SOCK_H_
#define ENGINE_SOCK_H_

void runServer(int port);

int connServer(const char *ip, int port);

void sendCMD(int sock);

#endif // ENGINE_SOCK_H_
