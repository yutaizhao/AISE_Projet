#include "./server_lib/server_lib.h"


void *handle_client(void *psocket) {
  int client_fd = *((int *)psocket);
    
    char buff[1024];
    struct string* client_string = (struct string*)malloc(sizeof(struct string));
    client_string->len = -1;
    client_string->type = 'N';
    client_string->key = NULL;
    client_string->value = NULL;
    client_string->next_KeyValue =NULL;
    
    while(1){
        memset(buff, 0, sizeof(buff));
        ssize_t receiv_client = recv(client_fd, &buff, sizeof(buff), 0);
        if(empty_checker(client_string)==-1){
            client_string = (struct string*)malloc(sizeof(struct string));
            client_string->len = -1;
            client_string->type = 'N';
            client_string->key = NULL;
            client_string->value = NULL;
            client_string->next_KeyValue =NULL;}
        
       if (receiv_client == 0) {
            printf("a client disconnected\n" );
            ssize_t client_quit = send(client_fd, buff, receiv_client, 0);
            break;
       } else if (receiv_client > 0) {
           printf("Received from client %s", buff);
       }
        
        
        if (strncmp(buff, "ping",4) == 0 && strlen(buff) ==5) { //ping0 = 5
            //if ping return pong
            pong(&client_fd);
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
        else
        {
            other(&client_fd);
        }
    }
    
    if(client_string!=NULL) {
        free(client_string);
        client_string = NULL;
        printf("a client_string is free \n");
    }
    
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
        (void)fprintf(stderr, "Failed to create server");
        exit(1);
    }
    
    mkdir("config",0777);

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
