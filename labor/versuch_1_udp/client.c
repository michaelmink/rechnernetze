#include <unistd.h> // für einige der Systemcalls
#include <ctype.h> // für toupper()
#include <sys/socket.h> // für Socket-Strukturen
#include <sys/types.h> // für Basistypen
#include <arpa/inet.h> // für Socketadressen 

#include <stdio.h>
#include <string.h>


int STATUS = 1;
#define BUFSIZE 1000

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

    while(STATUS){

        // Nutzereingabe (ASCII-Text)
        char buffer[BUFSIZE];
        fgets(buffer, BUFSIZE, stdin);
        // handle end of line \n
        char buffer_new[BUFSIZE];
        memcpy(buffer_new, buffer, BUFSIZE-1);

        printf("Eingabe: %s\n", buffer);
        if (strcmp(buffer, "exit\n") == 0){
            STATUS = 0;
        }

        // sendto
        int sendto_out = sendto(socket_fd, buffer_new, sizeof(buffer_new), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
        if (sendto_out == -1){
            perror("Fehler beim Senden.");
            return -1;
        }

    }
    // close
    close(socket_fd);
}
