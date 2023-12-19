#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>


int main(int argc, char ** argv)
{
    if(argc < 3)
    {
        fprintf(stderr, "USAGE: %s [HOST] [PORT]\n", argv[0]);
        return 1;
    }


    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;


    struct addrinfo *ret = NULL;

    if(getaddrinfo(argv[1], argv[2], &hints, &ret) < 0 )
    {
        perror("getaddrinfo");
        return 1;
    }

    struct addrinfo *tmp = NULL;

    int sock = -1;
    int success = 0;

    for(tmp = ret; tmp != NULL; tmp = tmp->ai_next)
    {
        sock = socket(tmp->ai_family, tmp->ai_socktype, tmp->ai_protocol);

        if(sock < 0)
        {
            continue;
        }

        if(connect( sock, tmp->ai_addr,  tmp->ai_addrlen) < 0 )
        {
            continue;
        }

        success = 1;
        break;
    }

    /* Connected ? */
    if(!success)
    {
        fprintf(stderr, "Pas connecté\n");
        return 1;
    }
    else
    {
        printf("Connected to %s:%s\n", argv[1], argv[2]);
        
    }
    
    
    /*
     client need to be added in white list
     */
    int login = 0 ;
    
    char pass_buff[12];
    char pass_rece[12];
    
    
    FILE *config = fopen("config.txt", "wx");

    if (config != NULL) {
        perror("config not existed, one is created, please set up your identifiant and communicate with the server \n");
        return 1;
    }

    fclose(config);
    
    FILE *file;
    file = fopen("config.txt", "r");
    if (file == NULL) {
        perror("open config failed\n");
        return 1;
    }
    
    if (fgets(pass_buff, sizeof(pass_buff), file) != NULL) {
        send(sock,pass_buff,sizeof(pass_buff),0);
        login = 1 ;
    } else {
        perror("Empty file\n");
        fclose(file);
        return 1;
    }
    
    while(login == 1){
        
        ssize_t receiv_server = recv(sock, pass_rece, sizeof(pass_rece), 0);
        
        if (receiv_server > 0) {
            login = 2 ;
        } else if (receiv_server == 0) {
            printf("Server disconnected\n");
            close(sock);
            return 0;
        } else {
            perror("recv\n");
            close(sock);
            return 0;
        }
        
    }
    
    if(login == 2){
        if(strcmp(pass_rece,"valid")){
            printf("login !!!\n");
        }else{
            printf("unvalid indentity\n" );
            fclose(file);
            return 1;
        }
    }
    
    
    fclose(file);
    
    pass_buff[strcspn(pass_buff, "\n")] = '\0';

    char buff[1024];
    char rece[1024];
    
    while(1){
        memset(buff, 0, sizeof(buff));
        memset(rece, 0, sizeof(rece));
        
        if(fgets(buff, 1024, stdin))
        {
                ssize_t sent_server = send(sock,buff,sizeof(buff),0); //send
 
                if(sent_server == -1) {
                    perror("failed");
                }else if (sent_server == 0) {
                    printf("disconnection");
                    break;
                }
                else {
                    printf("[%s] : %s", pass_buff, buff);
                }
                    
                ssize_t receiv_server = recv(sock, rece, sizeof(rece), 0); //recerive
                
                if (receiv_server > 0) {
                    printf("[server] : %s", rece);
                } else if (receiv_server == 0) {
                    printf("[server] : disconnected\n");
                    break;
                } else {
                    perror("recv");
                    break;
                }
                
        }
            
    }


    close(sock);
    return 0;
}


