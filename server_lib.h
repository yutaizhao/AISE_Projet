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

int findAndUpdateKey(char* key_value);
int check_format(char* key_value);

void pong(int* fd);
void set(int* fd,char* buf);
void get(int* fd);
void other(int* fd);

