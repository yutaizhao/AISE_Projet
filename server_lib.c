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
    ssize_t client_done = send(*fd, "unknown command\n", strlen("unknown command\n"), 0);
    if (client_done == -1) {
        perror("Response failed\n");
    } else {
        printf("[%s] : sent an unknown command\n",Id );
    }
}


int identification(int* fd, char* Id){

    int login = 0;
    char pass_rece[1024];
    
    FILE *file;
    file = fopen("server_config.txt", "r");
    if (file == NULL) {
        perror("open config failed\n");
        exit(1);;
    }
    
    //receive from client his Id
    while(login == 0){
        memset(pass_rece, 0, sizeof(pass_rece));
        ssize_t client_indentity = recv(*fd, pass_rece, sizeof(pass_rece), 0);
        pass_rece[strcspn(pass_rece, "\n")] = '\0';
        
        if (client_indentity > 0) {
            login = 1;
        } else {
            printf("a client disconnected\n" );
            ssize_t client_quit = send(*fd, pass_rece, client_indentity, 0);
            fclose(file);
        }
    }
    
    //compare his id with all id in whitelist
    char line[1024];
    memset(line, 0, sizeof(line));
    
    while (fgets(line,sizeof(line),file) != NULL) {
        line[strcspn(line, "\n")] = '\0';
        
        if(strcmp(line, pass_rece) == 0) {
            printf("a client connected : %s \n",line);
            strcpy(Id,pass_rece);
            login = 2 ;
            break;
        }
    }
    
    //answer client
    if(login == 2){
        fclose(file);
        send(*fd, "valid\n", strlen("valid\n"), 0);
    }else{
        fclose(file);
        send(*fd, "unvalid\n", strlen("unvalid\n"), 0);
    }
    
    return login;
}
