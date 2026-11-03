#include <unistd.h> // für einige der Systemcalls
#include <ctype.h> // für toupper()
#include <sys/socket.h> // für Socket-Strukturen
#include <sys/types.h> // für Basistypen
#include <arpa/inet.h> // für Socketadressen 

#include <stdio.h>
#include <string.h>

#define BUFSIZE 1000
#define MAX_CLIENTS 10

int get_len(unsigned char buffer[]);


// struc für source spezifisches Sammeln und Auswerten
typedef struct {
    char ip_addr;
    int min;
    int max;
    double mean;
} Stats;



int main() {

    printf("Starte Server...\n");

    // socket
    int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd == -1){
        perror("Fehler bei der Socketerstellung.");
        return -1;
    }

    // own_addr
    struct sockaddr_in own_addr;
    own_addr.sin_family = AF_INET;
    own_addr.sin_port = htons(3322);
    own_addr.sin_addr.s_addr = inet_addr("127.0.0.1");


    // bind
    int bind_status = bind(socket_fd, (struct sockaddr*)&own_addr, sizeof(own_addr));
    if (bind_status == -1){
        perror("Fehler beim Bind.");
        return -1;
    }

    while(1){

        // receive
        unsigned char buffer[BUFSIZE] = {0};
        struct sockaddr_in source_addr;
        unsigned int source_addr_len = sizeof(source_addr);
        int rec_status = recvfrom(socket_fd, buffer, sizeof(buffer), 0, (struct sockaddr*)&source_addr, &source_addr_len);
        if (rec_status == -1){
            perror("Fehler beim Empfang.");
            return -1;
        }

        char *source_ip = inet_ntoa(source_addr.sin_addr);
        unsigned short source_port = ntohs(source_addr.sin_port);
        
        
        printf("Empfangene Nachricht von %s:%d: %s (len: %d)\n", source_ip, source_port, buffer, get_len(buffer));
    
    }
}

int get_len(unsigned char buffer[]){
    int counter = 0;
    while(1){
        if (buffer[counter] == 10){
            return counter;
        }
        if (counter == BUFSIZE){
            return 0;
        }
        counter++;
    }
}