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

int SET(char* key_value);
char* GET(char* key_value);
int check_SET_format(char* buff);
int check_GET_format(char* buff);

void pong(int* fd);
void set(int* fd,char* buf);
void get(int* fd,char* buf);
void other(int* fd);

