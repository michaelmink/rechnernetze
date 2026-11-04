#include "common.h"

// get_or_create_client_entry
void get_or_create_client_entry(Stats stats[], char client_id[IP_ADDR_LEN+PORT_ADDR_LEN], int len, char *msg, int msglen){
    int FOUND = 0;
    // lookup client_id (if exists)
    for (int i=0; i<MAX_CLIENTS; i++){
        if (strcmp(stats[i].client_id, client_id) == 0){
            FOUND = 1;
            // increment counter
            stats[i].count++;
            // update sum
            stats[i].sum = stats[i].sum + len;
            // update max
            if (stats[i].max < len){
                stats[i].max = len;
            }
            // update min
            if (stats[i].min > len){
                stats[i].min = len;
            }
            // update mean
            stats[i].mean = (double)stats[i].sum / stats[i].count;

            snprintf(msg, msglen, " LEN= %d (History: Min=%d Max=%d Avg=%.2f)\n", len, stats[i].min, stats[i].max, stats[i].mean);
        }
    }
    if (FOUND == 0) {
        // init new entry
        int j;
        for (j=0; j<MAX_CLIENTS; j++){
            if (strcmp(stats[j].client_id, "") == 0){
                //printf("found index %d.\n", j);
                break;
            }
        }
        if (j == MAX_CLIENTS) {
            snprintf(msg, msglen, "Server voll (max. %d Clients)\n", MAX_CLIENTS);
            return;
        }
        
        snprintf(stats[j].client_id, IP_ADDR_LEN+PORT_ADDR_LEN, "%s", client_id);
        stats[j].count = 1;
        stats[j].sum = len;
        stats[j].min = len;
        stats[j].max = len;
        stats[j].mean = len;
        snprintf(msg, msglen, " LEN= %d (History: Min=%d Max=%d Avg=%.2f)\n", len, stats[j].min, stats[j].max, stats[j].mean);
    }
}
