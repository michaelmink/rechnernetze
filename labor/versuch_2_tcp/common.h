#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#define IP "127.0.0.1"
#define PORT 3322
#define RECV_BUFSIZE 65536 // 64 KB
#define MAX_LEN 10000
#define OUTPUT_MODE 0

static inline struct sockaddr_in make_addr(void){
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = inet_addr(IP);
    return addr;
}

int sendx(int sender_socket, void *buffer, int buflen, int flags);
int recvx(int socket_fd, void *buffer, int buflen, int flags);
void output_whole_segment(void *buffer, int buflen, int RECV_NO);
void output_compressed_segment(void *buffer, int buflen, int RECV_NO);

#endif