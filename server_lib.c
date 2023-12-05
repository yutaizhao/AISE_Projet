#include "server_lib.h"

void pong(int* fd){
        ssize_t client_pong = send(*fd, "pong\n", strlen("pong\n"), 0);
        if (client_pong == -1) {
            perror("Response failed\n");
        } else {
            printf("Sent to client: pong\n" );
        }
}

int check_SET_format(char* buff){
    if(buff[3]!=' '){
        return 0;
    }
    
    int cnt = 0;
    char cpybuff[1024];
    strcpy(cpybuff, buff); //neceessaire sinon buff sera modifie
    
    char* str = strtok(cpybuff, " ");
    while(str != NULL){
        str = strtok(NULL, " ");
        cnt = cnt +1 ;
    }
    
    if(cnt != 3){
        return 0;
    }
    
    return 1;
}

int check_GET_format(char* buff){
    if(buff[3]!=' '){
        return 0;
    }
    
    int cnt = 0;
    char cpybuff[1024];
    strcpy(cpybuff, buff);
    char* str = strtok(cpybuff, " ");
    while(str != NULL){
        str = strtok(NULL, " ");
        cnt = cnt +1 ;
    }
    if(cnt!= 2){
        return 0;
    }
    
    return 1;
}

int SET(char* key_value){
    
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

char* GET(char* key_value){
    
    FILE *file;
    file = fopen("./config/config.txt", "r+");
    if (file == NULL) {
        perror("open file failed\n");
        return NULL;
    }
    //line from origin filee
    char line[1024];
    memset(line, 0, sizeof(line));
    char* res = NULL;
    
    // key
    char *targetKey = strtok(key_value, " ");
    targetKey[strcspn(targetKey, "\n")] = '\0';
    printf("target : %s \n",targetKey);
    
    while (fgets(line,sizeof(line),file) != NULL) {
        //old key_value
        char *key = strtok(line, " ");
        char *value = strtok(NULL, " ");
        value[strcspn(value, "\n")] = '\0';
        
        if(strcmp(key, targetKey) == 0) {
            printf("valuee : %s \n",value);
            res = strdup(value);
            break;
        }
    }
    fclose(file);
    printf("res : %s \n",res);
    return res;
}

void set(int* fd,char* buf){
    
    char* kv = buf +4;
    int k = SET(kv);
    
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


void get(int* fd,char* buf){
    
    char* kv = buf +4;
    char* k = GET(kv);
    
    if (k == NULL) {
        if(send(*fd, "Can not find GET key\n", strlen("Can not find GET key\n"), 0)==-1) {perror("Response failed\n");}
        free(k);
    } else {
        if(send(*fd, k, strlen(k), 0)==-1) {perror("Response failed\n");}
        free(k);
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
