#include "string.h"


char type_checker(char* value){

    char* endptr; //end of converted string
    long int_value = strtol(value, &endptr, 10);

    if (*endptr == '\0' || *endptr == '\n') {
        printf("The value is an integer: %ld\n", int_value);
        return 'd';
    } else {
        //if what is converted is not all
        //then its not int
        char* endptr;
        double float_value = strtod(value, &endptr);

        if (*endptr == '\0' || *endptr == '\n') {
            printf("The value is a float: %lf\n", float_value);
            return 'f';
        }
    }
    //if what is converted is not all
    //then its not float neitheer
    printf("The value is a string: %s", value);
    return 's';
}



int check_SET_format(char* buff){
    if(buff[3]!=' '){
        return 0;
    }
    
    int cnt = 0;
    char cpybuff[1024];
    strcpy(cpybuff, buff); //neceessaire sinon buff sera modifie
    
    char* str = strtok(cpybuff, " ");
    while(str != NULL){
        str = strtok(NULL, " ");
        cnt = cnt +1 ;
    }
    
    if(cnt != 3){
        return 0;
    }
    
    return 1;
}

int check_GET_format(char* buff){
    if(buff[3]!=' '){
        return 0;
    }
    
    int cnt = 0;
    char cpybuff[1024];
    strcpy(cpybuff, buff);
    char* str = strtok(cpybuff, " ");
    while(str != NULL){
        str = strtok(NULL, " ");
        cnt = cnt +1 ;
    }
    if(cnt!= 2){
        return 0;
    }
    
    return 1;
}



struct string* GET(char* given_Key,struct string* list){
    
    struct string* current_KeyValue = list; //eviter
    
    while ((*current_KeyValue).next_KeyValue != NULL) {
        if(strcmp((*current_KeyValue).key, given_Key) == 0) {
            printf("valuee : %s \n",(*current_KeyValue).value);
            return current_KeyValue;
        }
        
        current_KeyValue =  (*current_KeyValue).next_KeyValue;
    }
    
    /*was forgetten*/
    if(strcmp((*current_KeyValue).key, given_Key) == 0) {
        printf("valuee : %s \n",(*current_KeyValue).value);
        return current_KeyValue;
    }
    
    
    return NULL;
}


void get(int* fd,char* buf, struct string* list){
    
    char* kv = buf +4;
    char* targetKey = strtok(kv, " ");
    targetKey[strcspn(targetKey, "\n")] = '\0';
    
    struct string* str = GET(targetKey,list);
    
    if (str == NULL) {
        if(send(*fd, "Can not find key\n", strlen("Can not find key\n"), 0)==-1) {perror("Response failed\n");}
    } else {
        char* v = strdup((*str).value);
        strcat(v,"\n");
        if(send(*fd, v, strlen(v), 0)==-1) {perror("Response failed\n");}
    }
}




struct string* SET(char* given_KeyValue, struct string* list){
    
    char *targetKey = strtok(given_KeyValue, " ");
    char *newValue = strtok(NULL, " ");
    newValue[strcspn(newValue, "\n")] = '\0';
    
    struct string* str = GET(targetKey,list);
    
    if(str != NULL){
        free((*str).value); //causing seg fault
        (*str).value = NULL;
        (*str).type = type_checker(newValue);
        (*str).value = strdup(newValue);
        (*str).len = strlen(newValue);
        
        return str;
    }else{
        struct string* new_str = (struct string*)malloc(sizeof(struct string));
        (*new_str).type = type_checker(newValue);
        (*new_str).len = strlen(newValue);
        (*new_str).value = strdup(newValue);
        (*new_str).key = strdup(targetKey);
        (*new_str).next_KeyValue = NULL;
        
        if (list == NULL) {
            list = new_str;  // list is null => new_str is the head
        } else {
            struct string* current = list;
            while (current->next_KeyValue != NULL) {
                current = current->next_KeyValue;
            }
            current->next_KeyValue = new_str;  // 链接到列表的末尾
        }
        
        
        return new_str;
        
    }
}

void set(int* fd,char* buf,struct string* list){
    
    char* kv = buf +4;
    struct string* k = SET(kv,list);
    
    
    if (k == NULL) {
        if(send(*fd, "Failed to SET\n", strlen("Failed to SET\n"), 0)==-1) {perror("Response failed\n");}
    } else {
        if(send(*fd, "SET done \n", strlen("SET done \n"), 0)==-1) {perror("Response failed\n");}
    }
}






