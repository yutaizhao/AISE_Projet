#include "./server_lib/server_lib.h"
struct string* public_cstring = NULL;
void *handle_client(void *psocket);
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

char* commun_data_path = "data/data.txt";

void start_server(char* port) {
    //preparing server
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    
    hints.ai_family = AF_UNSPEC; // unspecified ipv4/6 all OK
    hints.ai_socktype = SOCK_STREAM; // for tcp
    hints.ai_flags = AI_PASSIVE; //listening
    
    struct addrinfo *aret = NULL;
    //END preparing server
    
    int ret = getaddrinfo(NULL, port, &hints, &aret);
    
    if (ret < 0) {
        perror("getaddrinfo");
        return ;
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

    printf("Server listening on port %s...\n", port);

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
}

int main(int argc, char **argv) {
    
    if (argc < 3) {
        (void)fprintf(stderr, "Usage %s [PORT1] [PORT2]\n", argv[0]);
        exit(1);
    }

    // 启动两个服务器，分别监听不同的端口
    pthread_t thread1, thread2;
    
    mkdir("data",0777);
    
    FILE *file = fopen("server_config.txt", "wx");
    
    if (file != NULL) {
        perror("config not existed, one is created, please set up, and rerun\n");
        exit(1);
    }
    
    fclose(file);
    
    public_cstring = (struct string*)malloc(sizeof(struct string));
    public_cstring->len = -1;
    public_cstring->type = 'N';
    public_cstring->key = NULL;
    public_cstring->value = NULL;
    public_cstring->next_KeyValue =NULL;

    if (pthread_create(&thread1, NULL, (void *)start_server, (void *)argv[1]) != 0) {
        perror("Error creating thread for server 1");
        exit(EXIT_FAILURE);
    }

    if (pthread_create(&thread2, NULL, (void *)start_server, (void *)argv[2]) != 0) {
        perror("Error creating thread for server 2");
        exit(EXIT_FAILURE);
    }

    // 主线程等待子线程结束
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    
    free(public_cstring);

    return 0;
}


void *handle_client(void *psocket) {
    int client_fd = *((int *)psocket);
    char* client_id = (char*)malloc(sizeof(char)*12);
    
    
    
    //Identification
    
    
    
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
    
    
    //setting private_path
    size_t len_path_dir = strlen("data/data_") + strlen(pass_rece) +1;
    char dir_path[len_path_dir];
    memset(dir_path, 0, sizeof(dir_path));
    strcat(dir_path, "data/data_");
    strcat(dir_path, pass_rece);
    mkdir(dir_path,0777);
    printf("%s\n",dir_path);
    size_t len_path = len_path_dir + strlen("/data_") + strlen(pass_rece) + strlen(".txt") + 1;
    // +1 for the null terminator
    char data_path[len_path];
    memset(data_path, 0, sizeof(data_path));
    strcat(data_path,dir_path);
    strcat(data_path, "/data_");
    strcat(data_path, pass_rece);
    strcat(data_path, ".txt");
    printf("%s\n",data_path);

    //setting private_path
    
    
    struct string* client_string = (struct string*)malloc(sizeof(struct string));
    client_string->len = -1;
    client_string->type = 'N';
    client_string->key = NULL;
    client_string->value = NULL;
    client_string->next_KeyValue =NULL;
    
    
    int current_string = 0;
    char buff[1024];
    
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
        
        
        
        //ping
        if (strncmp(buff, "ping",4) == 0 && strlen(buff) ==5) { //ping0 = 5
            //pong
            
            pong(&client_fd,client_id);
            printf("current data base is %d \n",current_string );
            
        }
        //set
        else if (strncmp(buff, "SET", 3) == 0 && strlen(buff) >4)
        {
            if(check_SET_format(buff)==0){
                send(client_fd, "Usage: SET [key] [value]\n", strlen("Usage: SET [key] [value]\n"), 0);
            }else{
                // set str
                switch (current_string){
                    case 0:
                        set(&client_fd, buff, client_string);
                        break;
                    case 1:
                        if (pthread_mutex_trylock(&mutex) == 0){
                            set(&client_fd, buff, public_cstring);
                            pthread_mutex_unlock(&mutex);
                        }else{
                            send(client_fd, "database is modifying by another user\n", strlen("database is modifying by another user\n"), 0);
                        }
                        break;
                }
                
            }
            
        }
        //get
        else if (strncmp(buff, "GET", 3) == 0 && strlen(buff) >4)
        {
            if(check_GET_format(buff)==0){
                send(client_fd, "Usage: GET [key]\n", strlen("Usage: GET [key]\n"), 0);
            }else{
                // get str
                switch (current_string){
                    case 0:
                        get(&client_fd, buff, client_string);
                        break;
                    case 1:
                        if (pthread_mutex_trylock(&mutex) == 0){
                            pthread_mutex_unlock(&mutex);
                            get(&client_fd, buff, public_cstring);
                        }else{
                            send(client_fd, "database is modifying by another user\n", strlen("database is modifying by another user\n"), 0);
                        }
                        
                        break;
                }
            }
            
        }
        //DEL
        else if (strncmp(buff, "DEL", 3) == 0 && strlen(buff) >4)
        {
            if(buff[3] != ' '){
                send(client_fd, "Usage: DEL [key]\n", strlen("Usage: DEL [key]\n"), 0);
            }else{
                // get str
                switch (current_string){
                    case 0:
                        del(&client_fd, buff, &client_string);
                        break;
                    case 1:
                        if (pthread_mutex_trylock(&mutex) == 0){
                            del(&client_fd, buff, &public_cstring);
                            pthread_mutex_unlock(&mutex);
                        }else{
                            send(client_fd, "database is modifying by another user\n", strlen("database is modifying by another user\n"), 0);
                        }
                        
                        break;
                }
            }
            
        }
        //save
        else if (strncmp(buff, "SAVE", 4) == 0 && strlen(buff) == 5)
        {
            // save
            switch (current_string){
                case 0:
                    save(&client_fd, client_string, data_path);
                    break;
                case 1:
                    if (pthread_mutex_trylock(&mutex) == 0){
                        save(&client_fd, public_cstring, commun_data_path);
                        pthread_mutex_unlock(&mutex);
                    }else{
                        send(client_fd, "database is modifying by another user\n", strlen("database is modifying by another user\n"), 0);
                    }
                    break;
            }
            
        }
        else if (strncmp(buff, "SORT", 4) == 0 && strlen(buff) == 5)
        {
            // sort
            switch (current_string){
                case 0:
                    sort(&client_fd, data_path, dir_path);
                    break;
                case 1:
                    if (pthread_mutex_trylock(&mutex) == 0){
                        sort(&client_fd, commun_data_path, "data");
                        pthread_mutex_unlock(&mutex);
                    }else{
                        send(client_fd, "database is modifying by another user\n", strlen("database is modifying by another user\n"), 0);
                    }
                    break;
            }
            
        }
        //select
        else if (strncmp(buff, "SELECT", 6) == 0 && strlen(buff) > 7)
        {
            if(buff[6] != ' '){
                send(client_fd, "Usage: SELECT [nb]\n", strlen("Usage: SELECT [nb]\n"), 0);
            }else{
                // select
                int database = atoi(buff+7);
                current_string = select_str(&client_fd, database);
                
            }
        }
        else
        {
            other(&client_fd, client_id);
        }
    }
    
    if(client_string!=NULL) {
        while(client_string!=NULL && client_string->next_KeyValue!=NULL){
            struct string* next = client_string->next_KeyValue;
            free(client_string);
            client_string = next;
        }
        free(client_string);
        printf("[%s] : client_string is free \n",client_id);
    }
    
    free(client_id);
    close(client_fd);
    return NULL;
}
