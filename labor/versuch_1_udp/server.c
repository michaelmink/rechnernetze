#include "common.h"


int main() {

    printf("Starte Server...\n");

    // init Stats
    Stats stats[MAX_CLIENTS] = {0};

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
        int rec_len = recvfrom(socket_fd, buffer, BUFSIZE - 1, 0, (struct sockaddr*)&source_addr, &source_addr_len);
        if (rec_len == -1){
            perror("Fehler beim Empfang.");
            return -1;
        }

        // define client_id as <source_ip>_<source_port>
        char *source_ip = inet_ntoa(source_addr.sin_addr);
        unsigned short source_port = ntohs(source_addr.sin_port);
        char client_id[IP_ADDR_LEN+PORT_ADDR_LEN];
        snprintf(client_id, sizeof(client_id), "%s_%d", source_ip, source_port);

        // print received msg and ip+port
        printf("%s (%d): %s\n", source_ip, source_port, buffer);

        // compute stats for client_id
        char msg[100];
        get_or_create_client_entry(stats, client_id, rec_len, msg, sizeof(msg));
        int idx_len = strcspn(msg, "\n");
        printf("%s --> LEN: %d, %s", client_id, idx_len, msg);

        // send
        int send_status = sendto(socket_fd, msg, idx_len, 0, (struct sockaddr*)&source_addr, sizeof(source_addr));
        if (send_status == -1){
            perror("Fehler beim Senden.");
            return -1;
        }
    }
}
