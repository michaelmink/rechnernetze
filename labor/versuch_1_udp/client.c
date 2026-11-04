#include "common.h"


int main() {

    // socket
    int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd == -1){
        perror("Fehler bei der Socketerstellung.");
        return -1;
    }

    // dest_addr
    struct sockaddr_in dest_addr;
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(3322);
    dest_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    int STATUS = 1;
    while(STATUS){

        // Nutzereingabe (ASCII-Text)
        char buffer[BUFSIZE];
        if (fgets(buffer, BUFSIZE, stdin) == NULL) break;

        // identify relevant part of buffer
        int idx_len = strcspn(buffer, "\n");
        buffer[idx_len] = '\0'; // "hallo\n\0" --> "hallo\0\0"
        if (strcmp(buffer, "Exit") == 0){
            STATUS = 0;
        }

        // sendto
        printf("Sending (LEN: %d): %s\n", idx_len, buffer);
        int sendto_out = sendto(socket_fd, buffer, idx_len, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
        if (sendto_out == -1){
            perror("Fehler beim Senden.");
            return -1;
        }

        // recvfrom
        char recv_buffer[BUFSIZE];
        struct sockaddr source_addr;
        unsigned int source_addr_len = sizeof(source_addr);
        int recv_len = recvfrom(socket_fd, recv_buffer, BUFSIZE-1, 0, (struct sockaddr *)&source_addr, &source_addr_len);
        if (recv_len == -1){
            perror("Fehler beim Empfangen.");
            return -1;
        }
        recv_buffer[recv_len] = '\0';
        printf("%s\n", recv_buffer);
    }
    // close
    close(socket_fd);
}
