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
        // receive
        RECV_NO ++;
        unsigned char buffer[RECV_BUFSIZE];
        int buflen = sizeof(buffer);
        int rec_out = recv(accept_socket, buffer, buflen, 0);
        if (rec_out == -1) {
            perror("Fehler beim Receive.");
            close(accept_socket);
            close(receiver_socket_fd);
            return -1;
        }
        if (rec_out == 0) {
            printf("Sender hat die Verbindung geschlossen.\n");
            close(accept_socket);
            close(receiver_socket_fd);
            return 0;
        }

        printf("RECV-NO: %d, LEN: %d, CONTENTS: %02x", RECV_NO, rec_out, buffer[0]);
        for (int k = 1; k < rec_out; k++) {
            if (buffer[k] != buffer[k-1]) {
                printf("..%02x", buffer[k]);
            }
        }
        printf("\n");
    }

    // close
    close(receiver_socket_fd);
    printf("... Receiver finished.\n");
}
