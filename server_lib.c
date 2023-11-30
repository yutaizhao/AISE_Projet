#include "server_lib.h"

void pong(int* fd){
        ssize_t client_pong = send(*fd, "pong\n", strlen("pong\n"), 0);
        if (client_pong == -1) {
            perror("Response failed\n");
        } else {
            printf("Sent to client: pong\n" );
        }
}

void set(int* fd,char* buf){
        FILE *file;
        file = fopen("./config/config.txt", "w+");
        if (file == NULL) {perror("open file failed\n");}
        
        char *set_info = (char *)malloc(sizeof(char)*(strlen(buf)-4));
        strcpy(set_info,buf+4);
        printf("%s",set_info);
        fprintf(file,"%s", set_info);
        free((void *)set_info);
        fclose(file);
        
        ssize_t client_done = send(*fd, "Done\n", strlen("Done\n"), 0);
        if (client_done == -1) {
            perror("Response failed\n");
        } else {
            printf("Done\n" );
        }
}

void other(int* fd){
    //else
    //eviter quee stdin soit bouch, cuz not send back
    ssize_t client_done = send(*fd, "Verifier votre commande\n", strlen("Verifier votre commande\n"), 0);
    if (client_done == -1) {
        perror("Response failed\n");
    } else {
        printf("BAD client commande\n" );
    }
}
