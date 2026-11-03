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
            // update max
            if (stats[i].max < len){
                stats[i].max = len;
            }
            // update min
            if (stats[i].min > len){
                stats[i].min = len;
            }
            // update mean
            stats[i].mean = ((stats[i].mean * (stats[i].count -1)) + len) / stats[i].count;

            snprintf(msg, 100, " LEN= %d (History: Min=%d Max=%d Avg=%f)", len, stats[i].min, stats[i].max, stats[i].mean);
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
        
        snprintf(stats[j].client_id, IP_ADDR_LEN+PORT_ADDR_LEN, "%s", client_id);
        stats[j].count = 1;
        stats[j].min = len;
        stats[j].max = len;
        stats[j].mean = len;
        snprintf(msg, 100, " LEN= %d (History: Min=%d Max=%d Avg=%f)", len, stats[j].min, stats[j].max, stats[j].mean);
    }
}
