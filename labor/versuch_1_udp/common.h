#include <unistd.h> // für einige der Systemcalls
#include <ctype.h> // für toupper()
#include <sys/socket.h> // für Socket-Strukturen
#include <sys/types.h> // für Basistypen
#include <arpa/inet.h> // für Socketadressen 

#include <stdio.h>
#include <string.h>

#define BUFSIZE 1000
#define MAX_CLIENTS 10
#define IP_ADDR_LEN 16
#define PORT_ADDR_LEN 5


// struc für source spezifisches Sammeln und Auswerten
typedef struct {
    char client_id[IP_ADDR_LEN+PORT_ADDR_LEN];
    int min;
    int max;
    int count;
    double mean;
} Stats;


int get_len(unsigned char buffer[]);
void get_or_create_client_entry(Stats stats[], char client_id[IP_ADDR_LEN+PORT_ADDR_LEN], int len, char *msg, int msglen);
