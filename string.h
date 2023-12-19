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
#include <sys/time.h>
#include <ctype.h> 


struct string{ // list of key_value
    
    int len; //without \0
    char type; //s = string, int = d, float =f, N/A = N
    char* key;
    char* value;
    
    struct string* next_KeyValue;
    //1+8+8+8 =25
    
};

/*
 COMMANDS
*/
int select_str( int* fd, int database); //SELECT, Redis-like command
void set(int* fd,char* buff, struct string* list); //SET, Redis-like command
void get(int* fd,char* buff, struct string* list); //GET, Redis-like command
void mget(int* fd,char* buff, struct string* list);//MGET, Redis-like command
void del(int* fd,char* buff, struct string** list);//DEL, Redis-like command
void incr(int* fd,char* buff,struct string* list); //INCR, Redis-like command
void save(int* fd, struct string* list, char* path); //SAVE, Redis-like command
void externalSort(int* fd, const char *dir_path, const char *id); //SORT, see README for more
void externalGet(int* fd, char* buff, const char *dir_path, const char *id); //FETCH, see README for more
//if have time implement get type and get len


/*
 USEFULL functions !
*/

//change a key-value, return the pointer to the changed key-value
struct string* SET(char* given_KeyValue, struct string* list);
//get a value, return the pointer to the seeked key-value
struct string* GET(char* given_Key, struct string* list);
//delete keys-values, return the pointer NULL if cannot find key
struct string* DEL(char* given_Key,struct string** list);
//increment a value, return the pointer to the incremented key-value
struct string* INCR(struct string* given_string);

/*Check commands format*/
int check_SET_format(char* buff);
int check_GET_format(char* buff);
int check_FETCH_format(char* buff);

char type_checker(char* value); //return the type of a value : s = string, int = d, float =f, empty = N
int empty_checker(struct string* list); //check if a string is empty

