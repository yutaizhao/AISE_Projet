#include "./server_lib/server_lib.h"


void *handle_client(void *psocket) {
    int client_fd = *((int *)psocket);
    char* client_id = (char*)malloc(sizeof(char)*12);
    
    
    FILE *file;
    file = fopen("server_config.txt", "r");
    if (file == NULL) {
        perror("open config failed\n");
        close(client_fd);
        free(client_id);
        return NULL;
    }
    
    int login = 0;
    char pass_rece[12];
    
    while(login == 0){
        memset(pass_rece, 0, sizeof(pass_rece));
        ssize_t client_indentity = recv(client_fd, pass_rece, sizeof(pass_rece), 0);
        
        if (client_indentity == 0) {
            printf("a client disconnected\n" );
            ssize_t client_quit = send(client_fd, pass_rece, client_indentity, 0);
            //清空缓存区
            fclose(file);
            close(client_fd);
            free(client_id);
            return NULL ;
        } else if (client_indentity > 0) {
            login = 1;
        }
    }
    
    //line from origin filee
    char line[12];
    memset(line, 0, sizeof(line));
    
    // indentity
    pass_rece[strcspn(pass_rece, "\n")] = '\0';
    
    while (fgets(line,sizeof(line),file) != NULL) {
        line[strcspn(line, "\n")] = '\0';
        
        if(strcmp(line, pass_rece) == 0) {
            printf("a client tried to login : %s \n",line);
            strcpy(client_id,line);
            login = 2 ;
            break;
        }
    }
    
    if(login == 2){
        send(client_fd, "valid\n", strlen("valid\n"), 0);
        fclose(file);
    }else{
        send(client_fd, "unvalid\n", strlen("unvalid\n"), 0);
        fclose(file);
        close(client_fd);
        free(client_id);
        return NULL;
    }
    
    //END indentification
    size_t len_path = strlen("data/data_") + strlen(pass_rece) + strlen(".txt") + 1; // +1 for the null terminator
    char data_path[len_path];
    strcat(data_path, "data/data_");
    strcat(data_path, pass_rece);
    strcat(data_path, ".txt");
    
    //end path
    
    char buff[1024];
    struct string* client_string = (struct string*)malloc(sizeof(struct string));
    client_string->len = -1;
    client_string->type = 'N';
    client_string->key = NULL;
    client_string->value = NULL;
    client_string->next_KeyValue =NULL;
    
    while(1){
        memset(buff, 0, sizeof(buff));
        ssize_t receiv_client = recv(client_fd, buff, sizeof(buff), 0);
        
        if (receiv_client == 0) {
            printf("[%s] : disconnected\n",client_id );
            ssize_t client_quit = send(client_fd, buff, receiv_client, 0);
            break;
        } else if (receiv_client > 0) {
            printf("[%s] : %s", client_id, buff);
        }
        
        
        if (strncmp(buff, "ping",4) == 0 && strlen(buff) ==5) { //ping0 = 5
            //if ping return pong
            pong(&client_fd,client_id);
        }
        //set
        else if (strncmp(buff, "SET", 3) == 0 && strlen(buff) >4)
        {
            if(check_SET_format(buff)==0){
                send(client_fd, "Usage: SET [key] [value]\n", strlen("Usage: SET [key] [value]\n"), 0);
            }else{
                // set str
                
                set(&client_fd, buff, client_string);
            }
            
        }
        //get
        else if (strncmp(buff, "GET", 3) == 0 && strlen(buff) >4)
        {
            if(check_GET_format(buff)==0){
                send(client_fd, "Usage: GET [key]\n", strlen("Usage: GET [key]\n"), 0);
            }else{
                // get str
                get(&client_fd, buff, client_string);
            }
            
        }
        //DEL
        else if (strncmp(buff, "DEL", 3) == 0 && strlen(buff) >4)
        {
            if(buff[3] != ' '){
                send(client_fd, "Usage: DEL [key]\n", strlen("Usage: DEL [key]\n"), 0);
            }else{
                // get str
                del(&client_fd, buff, &client_string);
            }
            
        }
        //save
        else if (strncmp(buff, "SAVE", 4) == 0 && strlen(buff) == 5)
        {
            // save
            save(&client_fd, client_string, data_path);
            
        }
        else
        {
            other(&client_fd, client_id);
        }
    }
    
    if(client_string!=NULL) {
        free(client_string);
        client_string = NULL;
        printf("[%s] : client_string is free \n",client_id);
    }
    
    free(client_id);
    close(client_fd);
    return NULL;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        (void)fprintf(stderr, "Usage %s [PORT]\n", argv[0]);
        exit(1);
    }
    
    //preparing server
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    
    hints.ai_family = AF_UNSPEC; // unspecified ipv4/6 all OK
    hints.ai_socktype = SOCK_STREAM; // for tcp
    hints.ai_flags = AI_PASSIVE; //listening
    
    struct addrinfo *aret = NULL;
    //END preparing server
    
    int ret = getaddrinfo(NULL, argv[1], &hints, &aret);
    
    if (ret < 0) {
        perror("getaddrinfo");
        return 1;
    }
    
    struct addrinfo *tmp = NULL;
    
    int sock = -1;
    int success = 0;
    //setting server
    for (tmp = aret; tmp != NULL; tmp = tmp->ai_next) {
        /* Parcours des choix*/
        sock = socket(tmp->ai_family, tmp->ai_socktype, tmp->ai_protocol); // create a socket
        
        if (sock < 0) {
            continue;
        }
        
        if (bind(sock, tmp->ai_addr, tmp->ai_addrlen)) {
            continue;
        }
        
        if (listen(sock, 5)) {
            continue;
        }
        
        /* All OK */
        success = 1;
        break;
    }
    //END setting server
    if (!success) {
        (void)fprintf(stderr, "Failed to create server \n");
        exit(1);
    }
    
    mkdir("data",0777);
    
    FILE *file = fopen("server_config.txt", "wx");

    if (file != NULL) {
        perror("config not existed, one is created, please set up, and rerun\n");
        exit(1);
    }

    fclose(file);
    
    while (1) {
        struct sockaddr addr;
        socklen_t len = sizeof(struct sockaddr);
        int ret = accept(sock, &addr, &len); //receive demande from client
        
        if (ret < 0) {
            perror("accept");
            continue;
        }
        
        /* We have a client */
        int *sockfd = malloc(sizeof(int));
        
        if (sockfd == NULL) {
            perror("malloc");
            continue;
        }
        
        *sockfd = ret;
        
        pthread_t th = 0;
        pthread_create(&th, NULL, handle_client, sockfd);
        
    }
    
    
    return 0;
}
