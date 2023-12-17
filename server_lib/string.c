#include "string.h"

int empty_checker(struct string* list){
    
    if (list->key == NULL && list->value ==NULL && list->type =='N' && list->len ==-1 &&list->next_KeyValue ==NULL ) {
        return 0;
    } else if(list == NULL){
        return -1;
    }
    return 1;
}

char type_checker(char* value){
    
    char* endptr; //end of converted string
    long int_value = strtol(value, &endptr, 10);
    
    if (*endptr == '\0' || *endptr == '\n') {
        printf("The %ld is an int \n", int_value);
        return 'd';
    } else {
        //if what is converted is not all
        //then its not int
        char* endptr;
        double float_value = strtod(value, &endptr);
        
        if (*endptr == '\0' || *endptr == '\n') {
            printf("The %lf is a float\n", float_value);
            return 'f';
        }
    }
    //if what is converted is not all
    //then its not float neitheer
    printf("The %s is a string \n", value);
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
    
    while (current_KeyValue != NULL) {
        if(strcmp((*current_KeyValue).key, given_Key) == 0) {
            return current_KeyValue;
        }
        if((*current_KeyValue).next_KeyValue == NULL){printf("is a NULL; \n");}
        current_KeyValue =  (*current_KeyValue).next_KeyValue;
        
    }
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


struct string* DEL(char* given_Key,struct string** list){
    
    if(empty_checker(*list)!=1){return NULL;}
    struct string* current_KeyValue = *list; //eviter de changer l'adresse de list
    struct string* current_KeyValue_save = *list; //eviter
    
    
    
    //begin key
    
    
    //if delete begin then next NOT exist
    if (strcmp(current_KeyValue->key, given_Key) == 0 &&(*current_KeyValue).next_KeyValue == NULL){
        free(*list) ;
        *list = (struct string*)malloc(sizeof(struct string));
        (*list)->len = -1;
        (*list)->type = 'N';
        (*list)->key = NULL;
        (*list)->value = NULL;
        (*list)->next_KeyValue = NULL;
        return *list;
    }
    
    //if delete begin then next exists
    if (strcmp(current_KeyValue->key, given_Key) == 0 &&(*current_KeyValue).next_KeyValue != NULL){
        
        *list =  current_KeyValue->next_KeyValue;
        free (current_KeyValue);
        current_KeyValue=NULL;
        current_KeyValue_save=NULL;
        return  *list;
    }
    
    //if NOT delete begin then next NOT exist
    if(current_KeyValue->next_KeyValue == NULL){
        return NULL;
    }
    current_KeyValue = current_KeyValue->next_KeyValue;
    
    
    
    
    //middle key
    
    
    
    
    while ((*current_KeyValue).next_KeyValue != NULL) {
        
        if(strcmp((*current_KeyValue).key, given_Key) == 0) {
            
            (*current_KeyValue_save).next_KeyValue=(*current_KeyValue).next_KeyValue;
            
            free ( current_KeyValue);
            current_KeyValue=NULL;
            return *list;
        }
        current_KeyValue_save=current_KeyValue;
        current_KeyValue =  (*current_KeyValue).next_KeyValue;
    }
    
    
    
    
    //last key
    
    
    
    if(strcmp((*current_KeyValue).key, given_Key) == 0) {
        free (current_KeyValue);
        current_KeyValue=NULL;
        current_KeyValue_save->next_KeyValue=NULL;
        return *list;
    }
    
    return NULL;
}

void del(int* fd,char* buff, struct string** list){
    
    int counter = 0 ; //循环计数器
    int counter_success = 0 ; //成功计数器
    
    char* kv = buff +4;
    char* targetKey = strtok(kv, " ");
    targetKey[strcspn(targetKey, "\n")] = '\0';
    
    while (targetKey != NULL) {
        targetKey[strcspn(targetKey, "\n")] = '\0';
        
        struct string* str = DEL(targetKey,list);
        if (str != NULL) {
            struct string* result = GET(targetKey, *list);
            
            if (result == NULL) {
                printf("recheck: key %s deleted successfully\n",targetKey);
            } else {
                printf("recheck: key %s deleted failed\n",targetKey);
            }
            counter_success = counter_success+1;
        }
        targetKey = strtok(NULL, " ");
        counter = counter +1;
    }
    if (counter_success == 0){
        if(send(*fd, "No key deleted\n", strlen("No key deleted\n"), 0)==-1) {perror("Response failed\n");}
    }else if (counter == counter_success) {
        if(send(*fd, "All passed key deleted\n", strlen("All passed key deleted\n"), 0)==-1) {perror("Response failed\n");}
    } else if(counter > counter_success) {
        if(send(*fd, "ATTENTION : some keys not found !\n", strlen("ATTENTION : some keys not found !\n"), 0)==-1) {perror("Response failed\n");}
    }
}


void save(int* fd, struct string* list, char* path){
    
    FILE *file;
    file = fopen(path, "w");
    if (file == NULL) {
        perror("open file failed\n");
        if(send(*fd, "Failed to SAVE\n", strlen("Failed to SAVE\n"), 0)==-1) {perror("Response failed\n");}
        return ;
    }
    
    if(empty_checker(list)!=1){
        if(send(*fd, "No data to SAVE\n", strlen("No data to SAVE\n"), 0)==-1) {perror("Response failed\n");}
        fclose(file);
        return ;
    }
    
    struct string* current_KeyValue = list; //eviter
    
    while (current_KeyValue != NULL) {
        
        fprintf(file, "%s %s\n",  (*current_KeyValue).key, (*current_KeyValue).value);
        current_KeyValue = current_KeyValue->next_KeyValue;
    }
    
    fclose(file);
    if(send(*fd, "DONE SAVE\n", strlen("DONE SAVE\n"), 0)==-1) {perror("Response failed\n");}
    
}



int select_str( int* fd, int database){
    if(database == 0 || database == 1){
        char str[strlen("is current string database\n")+4];
        sprintf(str, "%d is current string database\n", database);
        send(*fd, str, strlen(str), 0);
        return database;
    }else{
        send(*fd, "unknown, back to default detabase\n", strlen("unknown, back to default detabase\n"), 0);
        return 0;
    }
}

