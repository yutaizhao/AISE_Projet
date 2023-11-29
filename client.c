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

    char buff[1024];
    char rece[1024];
    
    while(1){
        memset(buff, 0, sizeof(buff));
        memset(rece, 0, sizeof(rece));
        
        if(fgets(buff, 1024, stdin))
        {
            if(strncmp(buff,"ping",4)==0){
                ssize_t ping = send(sock,&buff,sizeof(buff),0); //send
                
                if(ping == -1) {
                    perror("failed");
                }
                    
                ssize_t pong = recv(sock, &rece, sizeof(rece), 0); //recerive
                
                if (pong > 0) {
                    buff[pong] = '\0'; // 用null结尾接收的数据
                    printf("Received from server: %s\n", rece);
                } else if (pong == 0) {
                    printf("Server disconnected\n");
                    break;
                } else {
                    perror("recv");
                    break;
                }
                
            }
            
        }
    }


    close(sock);
    return 0;
}
