#include <fcntl.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>

void *handle_client(void *psocket) {
  int client_fd = *((int *)psocket);
    
    char buff[1024];
    
    while(1){
        memset(buff, 0, sizeof(buff));
        ssize_t receiv_client = recv(client_fd, &buff, sizeof(buff), 0);
        printf("Received from client: %s", buff);
        
        //if ping return pong
        if (strncmp(buff, "ping",4) == 0 && strlen(buff) ==5) { //ping0 = 5
            ssize_t client_pong = send(client_fd, "pong", strlen("pong"), 0);
            if (client_pong == -1) {
                perror("Response failed\n");
            } else {
                printf("Sent to client: pong\n" );
            }
        } 
        else if (strncmp(buff, "SET", 3) == 0 && strlen(buff) >4 )
        {
            
            FILE *file;
            file = fopen("config.txt", "w+");
            if (file == NULL) {perror("open file failed\n");}
            
            char *set_info = (char *)malloc(sizeof(char)*(strlen(buff)-4));
            strcpy(set_info,buff+4);
            printf("%s",set_info);
            fprintf(file,"%s", set_info);
            free((void *)set_info);
            fclose(file);
            
            ssize_t client_done = send(client_fd, "Done", strlen("Done"), 0);
            if (client_done == -1) {
                perror("Response failed\n");
            } else {
                printf("Done\n" );
            }
        }
        else{
            //else
            //eviter quee stdin soit bouch, cuz not send back
            ssize_t client_done = send(client_fd, "Verifier votre commande\n", strlen("Verifier votre commande\n"), 0);
            if (client_done == -1) {
                perror("Response failed\n");
            } else {
                printf("BAD client commande\n" );
            }
        }
    }

  //close(client_fd);
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

  while (1) {
    struct sockaddr addr;
    socklen_t len = sizeof(struct sockaddr);
    int ret = accept(sock, &addr, &len); //receive demande from client

    if (ret < 0) {
      perror("accept");
      exit(1);
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
