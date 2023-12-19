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
    char type; //s = string, int = d, float =f, non = N
    char* key;
    char* value;
    
    struct string* next_KeyValue;
    //1+8+8+8 =25
    
};

/*
 COMMANDS
*/
int select_str( int* fd, int database); //SELECT
void set(int* fd,char* buff, struct string* list);
void get(int* fd,char* buff, struct string* list);
void del(int* fd,char* buff, struct string** list);
void save(int* fd, struct string* list, char* path); //SAVE
void externalSort(int* fd, const char *dir_path, const char *id); //SORT
void externalGet(int* fd, char* buff, const char *dir_path, const char *id); //FETCH
//if have time implement get type and get len


/*
 USEFULL functions
*/
struct string* SET(char* given_KeyValue, struct string* list);
struct string* GET(char* given_Key, struct string* list);
struct string* DEL(char* given_Key,struct string** list);
int check_SET_format(char* buff);
int check_GET_format(char* buff);
int check_FETCH_format(char* buff);
char type_checker(char* value);
int empty_checker(struct string* list);





/*
struct string_space{
    
    struct string* head_str;
    
    struct string* private_str;
    
    //...
    
    struct string* public_str;
    
    //struct string* public_str_2;

};
 
 void select_str(struct string_space* space, int* fd, int mode, struct string* str_pub, struct string* str_priv){
     if(mode == 0){
         space->head_str = str_priv;
         send(*fd, "private\n", strlen("private\n"), 0);
     }else if(mode == 1){
         space->head_str = str_pub;
         send(*fd, "public\n", strlen("public\n"), 0);
     }else{
         send(*fd, "unknown\n", strlen("unknown\n"), 0);
     }
 }

*/
