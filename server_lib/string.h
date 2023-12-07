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


struct string{ // list of key_value
    
    int len;
    char type; //s = string, int = d, float =f;
    char* key;
    char* value;
    
    struct string* next_KeyValue;
    //1+8+8+8 =25
    
};



char type_checker(char* value);
int empty_checker(struct string* list);

void set(int* fd,char* buff, struct string* list);
void get(int* fd,char* buff, struct string* list);
struct string* SET(char* given_KeyValue, struct string* list);
struct string* GET(char* given_Key, struct string* list);
int check_SET_format(char* buff);
int check_GET_format(char* buff);
