#include "server_lib.h"

void pong(int* fd, char* Id){
        ssize_t client_pong = send(*fd, "pong\n", strlen("pong\n"), 0);
        if (client_pong == -1) {
            perror("Response failed\n");
        } else {
            printf("[server] to [%s]: pong\n", Id );
        }
}


void other(int* fd, char* Id){
    //else
    //eviter quee stdin soit bouch, cuz not send back
    ssize_t client_done = send(*fd, "unknown commande\n", strlen("unknown commande\n"), 0);
    if (client_done == -1) {
        perror("Response failed\n");
    } else {
        printf("[%s] : has send a unknown commande\n",Id );
    }
}


