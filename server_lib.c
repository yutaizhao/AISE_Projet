#include "server_lib.h"


int findAndUpdateKey(char* key_value){
    
    int foundkey = 0; //indicating result
    
    FILE *file;
    file = fopen("./config/config.txt", "r+");
    if (file == NULL) {
        perror("open file failed\n");
        return -1;
    }
    FILE *tempFile = fopen("./config/temp.txt", "w");
    if (tempFile == NULL) {
        perror("open file failed\n");
        return -1;
    }
    
    //line from origin filee
    char line[1024];
    memset(line, 0, sizeof(line));
    
    //new key_value
    char *targetKey = strtok(key_value, " ");
    char *newValue = strtok(NULL, " ");
    newValue[strcspn(newValue, "\n")] = '\0';
    
    while (fgets(line,sizeof(line),file) != NULL) {
        //old key_value
        char *key = strtok(line, " ");
        char *value = strtok(NULL, " ");
        value[strcspn(value, "\n")] = '\0';
        
        if(strcmp(key, targetKey) == 0) {
            fprintf(tempFile, "%s %s\n", targetKey, newValue);
            foundkey = 1;
        } else {
            fprintf(tempFile, "%s %s\n", key, value);
        }
    }
    fclose(file);
    fclose(tempFile);
    
    // delete origin
        if (remove("./config/config.txt") != 0) {
            perror("delete failed\n");
            return -1;
        }

        // rename temp
        if (rename("./config/temp.txt", "./config/config.txt") != 0) {
            perror("rename failed\n");
            return -1;
        }
    
    //if key non exist, append
    if (foundkey== 0){
        FILE *file;
        file = fopen("./config/config.txt", "a");
        if (file == NULL) {
            perror("open file failed\n");
            return -1;
        }
        
        fprintf(file,"%s %s\n", targetKey, newValue);
        foundkey = 2;
        fclose(file);
    }
    return foundkey;
}



void pong(int* fd){
        ssize_t client_pong = send(*fd, "pong\n", strlen("pong\n"), 0);
        if (client_pong == -1) {
            perror("Response failed\n");
        } else {
            printf("Sent to client: pong\n" );
        }
}

void set(int* fd,char* buf){
    
    char* kv = buf +4;
    int k = findAndUpdateKey(kv);
    
    if (k == -1) {
        if(send(*fd, "Error while SET\n", strlen("Error while SET\n"), 0)==-1) {perror("Response failed\n");}
    } else if (k == 0) {
        if(send(*fd, "Failed to SET\n", strlen("Failed to SET\n"), 0)==-1) {perror("Response failed\n");}
    } else if (k == 1) {
        if(send(*fd, "Updated\n", strlen("Updated\n"), 0)==-1){ perror("Response failed\n");}
    }else if (k == 2) {
        if(send(*fd, "Set done\n", strlen("Set done\n"), 0)==-1) { perror("Response failed\n");}
    }
}

void other(int* fd){
    //else
    //eviter quee stdin soit bouch, cuz not send back
    ssize_t client_done = send(*fd, "unknown commande\n", strlen("unknown commande\n"), 0);
    if (client_done == -1) {
        perror("Response failed\n");
    } else {
        printf("client has send a unknown commande\n" );
    }
}
