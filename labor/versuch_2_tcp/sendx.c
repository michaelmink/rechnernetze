#include "common.h"

int main() {
    
    printf("Launching Sender ...\n");

    // socket
    int sender_socket = socket(AF_INET, SOCK_STREAM, 0); // bei Fehler -1
    if (sender_socket == -1){
        perror("Fehler bei Socketerstellung.");
        return -1;
    }

    // connect
    struct sockaddr_in server_addr = make_addr();
    int connect_out = connect(sender_socket, (struct sockaddr *) &server_addr, sizeof(server_addr)); // bei Fehler -1

    if (connect_out == -1) {
        perror("No connection possible.");
        return -1;
    }

    while (1) {
        // Benutzereingabe
        int count;
        int len;
        
        printf("Count und Len eingeben:\n");
        while (scanf("%d %d", &count, &len) != 2){
            printf("Uengueltige Eingabe. Try again.\n");
            int c;
            while ((c = getchar()) != '\n' && c != EOF);  // falsche Eingabe aus stdin verwerfen
            continue;
        }
        if (len<=0 || len>=MAX_LEN) {
            printf("Laenge len nicht im erlaubten Bereich. Try again.\n");
            continue;
        }

        // send
        for (int i=0; i<count; i++){
            unsigned char buffer[len];
            for (int j=0; j<len; j++){
                buffer[j] = i % 256;
            }
            sendx(sender_socket, buffer, len, 0);
        }    
    }

    // close
    close(sender_socket);
    printf("... Sender finished.\n");

    return 0;
}

int sendx(int socket_fd, void *buffer, int buflen, int flags){
    // extend PDU by 2-Byte Lengthfield
    unsigned char pdu[buflen + 2];
    uint16_t len_net = htons(buflen);
    
    memcpy(pdu, &len_net, 2);
    memcpy(pdu + 2, buffer, buflen);
    
    // send with new header
    return send(socket_fd, pdu, buflen + 2, flags);
}
