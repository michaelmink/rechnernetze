#include "common.h"

int main() {
    printf("Launching Receiver ...\n");

    // socket
    int receiver_socket_fd = socket(AF_INET, SOCK_STREAM, 0); // bei Fehler -1
    if (receiver_socket_fd == -1) {
        perror("Fehler in der Socketerstellung.");
        return -1;
    }

    // bind
    struct sockaddr_in receiver_addr = make_addr();
    int bind_out = bind(receiver_socket_fd, (struct sockaddr *) &receiver_addr, sizeof(receiver_addr)); // bei Fehler -1
    if (bind_out == -1) {
        perror("Fehler beim Bind.");
        return -1;
    }

    // listen
    int listen_out = listen(receiver_socket_fd, 5); // bei Fehler -1
    if (listen_out == -1) {
        perror("Fehler beim Listen.");
        return -1;
    }

    // accept
    struct sockaddr_in source_addr;
    unsigned int source_addr_len = sizeof(source_addr);
    int accept_socket = accept(receiver_socket_fd, (struct sockaddr*)&source_addr, &source_addr_len); // bei Fehler -1
    if (accept_socket == -1) {
        perror("Fehler beim Accept.");
        close(receiver_socket_fd);
        return -1;
    }
    printf("Wer meldet sich: %s:%d\n", inet_ntoa(source_addr.sin_addr), ntohs(source_addr.sin_port));

    int RECV_NO = 0;
    while (1) {
        RECV_NO ++;
        // receive
        unsigned char buffer[RECV_BUFSIZE];
        int buflen = sizeof(buffer);
        int recx_out = recvx(accept_socket, buffer, buflen, 0);

        if (recx_out == -1) {
            perror("Fehler beim Receive.");
            close(accept_socket);
            close(receiver_socket_fd);
            return -1;
        }
        if (recx_out == 0) {
            printf("Sender hat die Verbindung geschlossen.\n");
            close(accept_socket);
            close(receiver_socket_fd);
            return 0;
        }

        switch (OUTPUT_MODE) {
        case 0:
            output_whole_segment(buffer, recx_out, RECV_NO);
            break;
        case 1:
            output_compressed_segment(buffer, recx_out, RECV_NO);
            break;
        }
    }

    // close
    close(receiver_socket_fd);
    printf("... Receiver finished.\n");
}

void output_whole_segment(void *buffer, int buflen, int RECV_NO) {
    unsigned char *buf = buffer;
    printf("RECV-NO: %d, LEN: %d, CONTENTS: ", RECV_NO, buflen);
    for (int k = 0; k < buflen; k++) {
        printf("%02x ", buf[k]);
    }
    printf("\n");
}

void output_compressed_segment(void *buffer, int buflen, int RECV_NO) {
    unsigned char *buf = buffer;
    printf("RECV-NO: %d, LEN: %d, CONTENTS: %02x", RECV_NO, buflen, buf[0]);
    for (int k = 1; k < buflen; k++) {
        if (buf[k] != buf[k-1]) {
            printf("..%02x", buf[k]);
        }
    }
    printf("\n");
}

int recv_all(int socket_fd, void *buffer, int buflen, int flags) {
    unsigned char *buf = buffer;
    int remaining = buflen;

    while (remaining > 0) {
        int rec_out = recv(socket_fd, buf + (buflen - remaining), remaining, flags);
        if (rec_out <= 0) return rec_out;
        remaining -= rec_out;
    }
    return buflen;
}

int recvx(int socket_fd, void *buffer, int buflen, int flags) {
    unsigned char *buf = buffer;

    // read first 2 Bytes into buffer
    int r = recv_all(socket_fd, buf, 2, flags);
    if (r <= 0) {
        return r;
    }

    int len = (buf[0] << 8) | buf[1];
    //printf("gelesene Len: %d --> ", len);

    // read segment into buffer using segment len
    int rec_out = recv_all(socket_fd, buf, len, flags);

    return rec_out;
}
