#include "string.h"

int empty_checker(struct string* list){
    
    if (list->key == NULL && list->value ==NULL && list->type =='N' && list->len ==-1 &&list->next_KeyValue ==NULL ) {
        return 0;
    } else if(list == NULL){
        list = (struct string*)malloc(sizeof(struct string));
        list->len = -1;
        list->type = 'N';
        list->key = NULL;
        list->value = NULL;
        list->next_KeyValue =NULL;
        return -1;
    }
    return 1;
}
    
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
    printf("The value is a string: %s\n", value);
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

    if(empty_checker(list)!=1){return NULL;}

    struct string* current_KeyValue = list; //eviter

    while ((*current_KeyValue).next_KeyValue != NULL) {
        printf("key : %s \n", (*current_KeyValue).key);
        printf("value : %s \n", (*current_KeyValue).value);
        if(strcmp((*current_KeyValue).key, given_Key) == 0) {
            return current_KeyValue;
        }
        printf("counter ! \n");
        if((*current_KeyValue).next_KeyValue == NULL){printf("is a NULL; \n");}
        printf("counter_2 ! \n");
        current_KeyValue =  (*current_KeyValue).next_KeyValue;
    }
    printf("passed !\n");
    
    if( empty_checker(current_KeyValue) != 1 ){printf("EMPTY !\n"); return NULL;}
    if( (*current_KeyValue).key == NULL ){printf("EMPTY key!\n"); return NULL;}
    if( given_Key == NULL ){printf("EMPTY str!\n"); return NULL;}
    printf("passed 2 !\n");
    /*was forgetten*/
    if(strcmp(current_KeyValue->key, given_Key) == 0) {
        printf("passed 3 !\n");
        return current_KeyValue;
    }
    
    printf("passed 4 !\n");
    return NULL;
}


void get(int* fd,char* buff, struct string* list){
    
    char* kv = buff +4;
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
    
    if(empty_checker(list) == -1){
        return NULL;
    }else if(empty_checker(list) == 0){
        (*list).type = type_checker(newValue);
        (*list).len = strlen(newValue);
        (*list).value = strdup(newValue);
        (*list).key = strdup(targetKey);
        (*list).next_KeyValue = NULL;
        return list;
    }else{
        struct string* str = GET(targetKey,list);
        if(str != NULL){
            free((*str).value); //causing seg fault
            (*str).value = NULL;
            (*str).type = type_checker(newValue);
            (*str).value = strdup(newValue);
            (*str).len = strlen(newValue);
            return str;
        }else{
            struct string* endptr = list;
            while (endptr->next_KeyValue != NULL) {
                endptr = endptr->next_KeyValue;
            }
            endptr->next_KeyValue = (struct string*)malloc(sizeof(struct string));
            endptr->next_KeyValue->type = type_checker(newValue);
            endptr->next_KeyValue->len = strlen(newValue);
            endptr->next_KeyValue->value = strdup(newValue);
            endptr->next_KeyValue->key = strdup(targetKey);
            endptr->next_KeyValue->next_KeyValue = NULL;
            return endptr->next_KeyValue;
        }
    }
}

void set(int* fd,char* buff,struct string* list){
    
    char* kv = buff +4;
    struct string* k = SET(kv,list);
    
    if (k == NULL) {
        if(send(*fd, "Failed to SET\n", strlen("Failed to SET\n"), 0)==-1) {perror("Response failed\n");}
    } else {
        if(send(*fd, "SET done \n", strlen("SET done \n"), 0)==-1) {perror("Response failed\n");}
    }
}


struct string* DEL(char* given_Key, struct string* list){
    
    //这里我删掉了get，相同功能可以与之后的循环一起完成，虽然这么写在特殊情况系统开销会更小，但在默认情况下循环太多了，系统开销太大
    if(empty_checker(list)!=1){return NULL;}
    
    struct string* current_KeyValue = list; //1
    struct string* current_KeyValue_save = list; //1
    
    //if the head is targetkey to be deleted
    //我从循环中挪出了这个判断，因为判断是消耗第二大的指令类型了，
    if (strcmp((*list).key, given_Key) == 0){
        if(empty_checker((*list).next_KeyValue)==-1){printf("A whole string has been delete \n");}//蛋蛋猫猫
        list =  (*list).next_KeyValue;
        free (current_KeyValue);
        current_KeyValue=NULL;
        if(GET(given_Key, list)==NULL){printf("recheck : delete successfully\n"); }
        printf("delete %s \n",given_Key);
        return list;
     }
    
    //指向第二项键值，如果可以的话，不然循环又会从第一项开始
    if(current_KeyValue->next_KeyValue == NULL){return NULL;}
    current_KeyValue = current_KeyValue->next_KeyValue;//2 and current_KeyValue_save at 1
    
    // if the head is not the targetkey
    while ((*current_KeyValue).next_KeyValue != NULL) {
        
        if(strcmp((*current_KeyValue).key, given_Key) == 0) {
            (*current_KeyValue_save).next_KeyValue=(*current_KeyValue).next_KeyValue;//
            free ( current_KeyValue);
            current_KeyValue=NULL;
            if(GET(given_Key, list)==NULL){printf("recheck : delete successfully\n"); }
            printf("delete %s \n",given_Key);
            return current_KeyValue_save;
        }
        current_KeyValue_save = current_KeyValue;
        current_KeyValue =  (*current_KeyValue).next_KeyValue;

    }
    
    //last key_value
    if(strcmp((*current_KeyValue).key, given_Key) == 0) {
        free (current_KeyValue);
        current_KeyValue=NULL;
        //理论上list == NULL的情况不会发生，否则将会与前面的代码矛盾
        if(GET(given_Key, list)==NULL){printf("recheck : delete successfully\n"); }
        printf("delete %s\n",given_Key);
        return list;
    }
    
    return NULL;
}

void del(int* fd,char* buff, struct string* list){
    
    char* kv = buff +4;
    char* targetKey = strtok(kv, " ");
    targetKey[strcspn(targetKey, "\n")] = '\0';
    
    struct string* str = DEL(targetKey,list);
    if (str == NULL) {
        if(send(*fd, "Can not delete key\n", strlen("Can not delete key\n"), 0)==-1) {perror("Response failed\n");}
    } else {
        strcat(targetKey,"deleted ! \n");
        if(send(*fd, targetKey, strlen(targetKey), 0)==-1) {perror("Response failed\n");}
    }
}
