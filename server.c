#include "server_lib.h"
#define COMMUN_DATA_PATH "./data/public/data_public"
#define COMMUN_DIR_PATH "./data/public"

void *handle_client(void *psocket);
void create_server(char* port);

struct string* public_cstring = NULL; //  a public string
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;



int main(int argc, char **argv) {
    
    if (argc < 3) {
        (void)fprintf(stderr, "Usage %s [PORT1] [PORT2]\n", argv[0]);
        exit(1);
    }
    
    //one thread for one server
    pthread_t thread1, thread2;
    
    mkdir("data",0777);
    mkdir("data/public",0777);
    
    FILE *file = fopen("server_config.txt", "wx");
    
    if (file != NULL) {
        perror("Config not existed, one is created, please set up, and rerun\n");
        exit(1);
    }
    
    fclose(file);
    
    /*
     Initialize a public string
     */
    
    public_cstring = (struct string*)malloc(sizeof(struct string));
    public_cstring->len = -1;
    public_cstring->type = 'N';
    public_cstring->key = NULL;
    public_cstring->value = NULL;
    public_cstring->next_KeyValue =NULL;
    
    if (pthread_create(&thread1, NULL, (void *)create_server, argv[1]) != 0) { // otherwaise got warning
        perror("Error creating server 1");
        exit(1);
    }
    
    if (pthread_create(&thread2, NULL, (void *)create_server, argv[2]) != 0) {
        perror("Error creating server 2");
        exit(1);
    }
    
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    
    
    /*
     Free a public string when all servers close
     */
    
    if(public_cstring!=NULL) {
        while(public_cstring!=NULL && public_cstring->next_KeyValue!=NULL){
            struct string* next = public_cstring->next_KeyValue;
            free(public_cstring);
            public_cstring = next;
        }
        free(public_cstring);
    }
    
    return 0;
}



/*
 Creat 2 sockets
 */

void create_server(char* port) {
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
        exit(1) ;
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



void *handle_client(void *psocket) {
    
    int client_fd = *((int *)psocket);
    char* client_id = (char*)malloc(sizeof(char)*12);
    
    //Client Identification
    int res = identification(&client_fd, client_id);
    if(res != 2){
        free(client_id);
        close(client_fd);
        return NULL;
    }
    
    
    /*
     setting path to save a user private string
     */
    
    size_t len_path_dir = snprintf(NULL, 0, "data/data_%s", client_id) + 1;
    char dir_path[len_path_dir];
    snprintf(dir_path, len_path_dir, "data/data_%s", client_id);
    mkdir(dir_path,0777);
    
    size_t len_path_file = snprintf(NULL, 0, "data/data_%s/data_%s", client_id,client_id) + 1;
    char data_path[len_path_file];
    snprintf(data_path, len_path_file, "data/data_%s/data_%s", client_id,client_id);
    
    /*
     Initialize a user private string
     */
    
    struct string* client_string = (struct string*)malloc(sizeof(struct string));
    client_string->len = -1;
    client_string->type = 'N';
    client_string->key = NULL;
    client_string->value = NULL;
    client_string->next_KeyValue =NULL;
    
    
    int current_string = 1;
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
            
            pong(&client_fd,client_id);
            printf("current data base is %d \n",current_string );
            
        }
        //set : set a key value
        else if (strncmp(buff, "SET", 3) == 0 && strlen(buff) >4)
        {
            if(check_SET_format(buff)==0){
                send(client_fd, "Usage: SET [key] [value]\n", strlen("Usage: SET [key] [value]\n"), 0);
            }else{ // format correct
                
                /*
                 cheeck detabase
                 */
                
                switch (current_string){
                    case 0:
                        set(&client_fd, buff, client_string);
                        break;
                    case 1:
                        if (pthread_mutex_trylock(&mutex) == 0){
                            set(&client_fd, buff, public_cstring);
                            pthread_mutex_unlock(&mutex); //lock the acceess cuz modifying
                        }else{
                            send(client_fd, "database is modifying by another user\n", strlen("database is modifying by another user\n"), 0);
                        }
                        break;
                        
                }
                
            }
            
        }
        //get : get the value of a key
        else if (strncmp(buff, "GET", 3) == 0 && strlen(buff) >4)
        {
            if(check_GET_format(buff)==0){
                send(client_fd, "Usage: GET [key]\n", strlen("Usage: GET [key]\n"), 0);
            }else{
                switch (current_string){
                    case 0:
                        get(&client_fd, buff, client_string);
                        break;
                    case 1:
                        if (pthread_mutex_trylock(&mutex) == 0){
                            pthread_mutex_unlock(&mutex); //users can acceeess to a database at the same time, unlock immediatly
                            get(&client_fd, buff, public_cstring);
                        }else{
                            send(client_fd, "database is modifying by another user\n", strlen("database is modifying by another user\n"), 0);
                        }
                        
                        break;
                }
            }
            
        }
        //mget : get a key or multi-keys
        else if (strncmp(buff, "MGET", 4) == 0 && strlen(buff) >5)
        {
            switch (current_string){
                case 0:
                    mget(&client_fd, buff, client_string);
                    break;
                case 1:
                    if (pthread_mutex_trylock(&mutex) == 0){
                        pthread_mutex_unlock(&mutex);
                        mget(&client_fd, buff, public_cstring);
                    }else{
                        send(client_fd, "database is modifying by another user\n", strlen("database is modifying by another user\n"), 0);
                    }
                    
                    break;
            }
            
        }
        //del : delete a key or multi-keys
        else if (strncmp(buff, "DEL", 3) == 0 && strlen(buff) >4)
        {
            if(buff[3] != ' '){
                send(client_fd, "Usage: DEL [key]\n", strlen("Usage: DEL [key]\n"), 0);
            }else{
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
        //incr : increment a key if key is int
        else if (strncmp(buff, "INCR", 4) == 0 && strlen(buff) >5)
        {
            if(buff[4] != ' '){
                send(client_fd, "Usage: INCR [key]\n", strlen("Usage: INCR [key]\n"), 0);
            }else{
                switch (current_string){
                    case 0:
                        incr(&client_fd, buff, client_string);
                        break;
                    case 1:
                        if (pthread_mutex_trylock(&mutex) == 0){
                            incr(&client_fd, buff, public_cstring);
                            pthread_mutex_unlock(&mutex);
                        }else{
                            send(client_fd, "database is modifying by another user\n", strlen("database is modifying by another user\n"), 0);
                        }
                        
                        break;
                }
            }
            
        }
        //save : save the database into a file
        else if (strcmp(buff, "SAVE\n") == 0 )
        {
            switch (current_string){
                case 0:
                    save(&client_fd, client_string, data_path);
                    break;
                case 1:
                    if (pthread_mutex_trylock(&mutex) == 0){
                        save(&client_fd, public_cstring, COMMUN_DATA_PATH);
                        pthread_mutex_unlock(&mutex);
                    }else{
                        send(client_fd, "database is modifying by another user\n", strlen("database is modifying by another user\n"), 0);
                    }
                    break;
            }
            
        }
        //sort external : sort a file data
        else if (strcmp(buff, "SORT\n") == 0)
        {
            switch (current_string){
                case 0:
                    externalSort(&client_fd, dir_path, client_id);
                    break;
                case 1:
                    if (pthread_mutex_trylock(&mutex) == 0){
                        externalSort(&client_fd, COMMUN_DIR_PATH , "public");
                        pthread_mutex_unlock(&mutex);
                    }else{
                        send(client_fd, "database is modifying by another user\n", strlen("database is modifying by another user\n"), 0);
                    }
                    break;
            }
            
        }
        // get external : get a value from a key from exterenal memory
        else if (strncmp(buff, "FETCH", 5) == 0 && strlen(buff) > 6)
        {
            if(check_FETCH_format(buff)==0){
                send(client_fd, "Usage: FETCH [key]\n", strlen("Usage: FETCH [key]\n"), 0);
            }else{
                switch (current_string){
                    case 0:
                        externalGet(&client_fd, buff, dir_path, client_id);
                        break;
                    case 1:
                        if (pthread_mutex_trylock(&mutex) == 0){
                            pthread_mutex_unlock(&mutex);
                            externalGet(&client_fd, buff, COMMUN_DIR_PATH , "public");
                        }else{
                            send(client_fd, "database is modifying by another user\n", strlen("database is modifying by another user\n"), 0);
                        }
                        break;
                }
            }
            
        }
        //select : change data base, 0 = private, 1: public
        else if (strncmp(buff, "SELECT", 6) == 0 && strlen(buff) > 7)
        {
            if(buff[6] != ' '){
                send(client_fd, "Usage: SELECT [nb]\n", strlen("Usage: SELECT [nb]\n"), 0);
            }else{
                int database = atoi(buff+7);
                current_string = select_str(&client_fd, database);
                
            }
        }
        else
        {
            other(&client_fd, client_id); // send unkown command
        }
    }
    
    /*
     free a string and client id
     */
    
    if(client_string!=NULL) {
        while(client_string!=NULL && client_string->next_KeyValue!=NULL){
            struct string* next = client_string->next_KeyValue;
            free(client_string);
            client_string = next;
        }
        free(client_string);
    }
    
    free(client_id);
    close(client_fd);
    return NULL;
}
