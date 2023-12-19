#include "string.h"

int empty_checker(struct string* list){
    
    if (list->key == NULL && list->value ==NULL && list->type =='N' && list->len ==-1 &&list->next_KeyValue ==NULL ) {
        return 0;
    } else if(list == NULL){
        return -1; //FATAL situation, impossible but in case
    }
    return 1;
}

char type_checker(char* value){
    
    char* endptr; //end of converted string
    long int_value = strtol(value, &endptr, 10);
    
    if (*endptr == '\0' || *endptr == '\n') {
        printf("(int)%ld\n", int_value);
        return 'd';
    } else {
        //if what is converted is not all
        //then its not int
        char* endptr;
        double float_value = strtod(value, &endptr);
        
        if (*endptr == '\0' || *endptr == '\n') {
            printf("(float)%lf\n", float_value);
            return 'f';
        }
    }
    //if what is converted is not all
    //then its not float neitheer
    printf("(string)%s\n", value);
    return 's';
}

int check_FETCH_format(char* buff){
    if(buff[5]!=' '){
        return 0;
    }
    int cnt = 0;
    char cpybuff[1024];
    strcpy(cpybuff, buff); //necessary to create cuz strtok can modify directly buff
    
    char* str = strtok(cpybuff, " ");
    while(str != NULL){
        str = strtok(NULL, " ");
        cnt = cnt +1 ;
    }
    
    if(cnt != 2){
        return 0;
    }
    
    return 1;
}



int check_SET_format(char* buff){
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
    
    struct string* current_KeyValue = list; //avoid to changee the string head adresse
    
    while (current_KeyValue != NULL) {
        if(strcmp((*current_KeyValue).key, given_Key) == 0) {
            return current_KeyValue;
        }
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
        
        // empty key-value in list, so add one
        
        (*list).type = type_checker(newValue);
        (*list).len = strlen(newValue);
        (*list).value = strdup(newValue);
        (*list).key = strdup(targetKey);
        (*list).next_KeyValue = NULL;
        return list;
    }else{
        struct string* str = GET(targetKey,list);
        if(str != NULL){
            
            // modify existing key-value in list
            
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
            
            // no repeted key in list, so add one
            
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
    struct string* current_KeyValue_save = *list;
    
    
    
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
    
    int counter = 0 ; //numbre of target keys
    int counter_success = 0 ; //deleted keys
    
    char* kv = buff +4;
    char* targetKey = strtok(kv, " ");
    targetKey[strcspn(targetKey, "\n")] = '\0';
    
    while (targetKey != NULL) {
        targetKey[strcspn(targetKey, "\n")] = '\0';
        
        //Delete
        struct string* str = DEL(targetKey,list);
        
       
        if (str != NULL) {
            
            //Using GET to recheck the key is really deleted or not, can
            struct string* result = GET(targetKey, *list);
            if (result == NULL) {
                printf("recheck: key %s deleted successfully\n",targetKey);
            } else {
                printf("recheck: key %s deleted failed\n",targetKey);
            }
            //Can delete this part if too slow
            
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


void mget(int* fd,char* buff, struct string* list){
    char* p = buff +5;
    int n=1;
    while (*p != '\n') {
        
        // 跳过空格
        while (*p == ' ') {
            ++p;
            ++n;
        }
        // 跳过键
        while (*p != ' ' && *p != '\n') {
            ++p;
        }
    }
    printf("%d\n",n);
    if (n>5) {
        if(send(*fd, "Error,too much keys\n", strlen("Error,too much keys\n"), 0)==-1) {perror("Response failed\n");}
        return ;
        }
    
    
    int MAX_MESSAGE_LENGTH=1024*10;
    int counter = 0 ; //循环计数器
    int counter_success = 0 ; //成功计数器
    char messagelist[MAX_MESSAGE_LENGTH];
    messagelist[0] = '\0';
    char* kv = buff +5;
    char* targetKey = strtok(kv, " ");
    targetKey[strcspn(targetKey, "\n")] = '\0';
    
    while (targetKey != NULL) {
        targetKey[strcspn(targetKey, "\n")] = '\0';
        struct string* str = GET(targetKey,list);
        if (str != NULL) {
            counter_success = counter_success+1;
            char* ms=strdup((*str).key);
            strcat(ms,",");
            strcat(ms,(*str).value);
            strncat(messagelist, ms, MAX_MESSAGE_LENGTH - strlen(messagelist) - 1);
            free(ms);
            strncat(messagelist, "  ", MAX_MESSAGE_LENGTH - strlen(messagelist) - 1);
        }
         else {
            strcat(messagelist,targetKey);
            strcat(messagelist," ");
            strncat(messagelist, "  doesn't exist.  ", MAX_MESSAGE_LENGTH - strlen(messagelist) - 1);
         }
            
            targetKey = strtok(NULL, " ");
            counter = counter +1;
    }

    if (counter_success == 0){
        if(send(*fd, "No key get\n", strlen("No key get\n"), 0)==-1) {perror("Response failed\n");}
    }else if (counter == counter_success) {
        strcat(messagelist,"All passed key get\n");
        if(send(*fd, messagelist, strlen(messagelist), 0)==-1) {perror("Response failed\n");}
    } else if(counter > counter_success) {
        strcat(messagelist,"ATTENTION : some keys not found !\n");
        if(send(*fd, messagelist, strlen(messagelist), 0)==-1) {perror("Response failed\n");}
    }
}


struct string* INCR(struct string* given_string){
    
    char type= (* given_string).type;
    if( type!='d'){return NULL;}
    else{
        int value = atoi((* given_string).value)+1;
        char buffer[1024];
        snprintf(buffer, sizeof(buffer), "%d", value);
        free((*given_string).value);
        (*given_string).value = strdup(buffer);
        return given_string;
    }
}

    
void incr(int* fd,char* buff,struct string* list){
    char* kv = buff +5;
    char* targetKey = strtok(kv, " ");
    targetKey[strcspn(targetKey, "\n")] = '\0';
    struct string* str = GET(targetKey,list);
    if (str == NULL) {
        if(send(*fd, "Cannot find key\n", strlen("Cannot find key\n"), 0)==-1) {perror("Response failed\n");}
    }
    struct string* newres =INCR(str);
    if (newres == NULL) {
        if(send(*fd, "This is not a int\n", strlen("This is not a int\n"), 0)==-1) {perror("Response failed\n");}
    }
    else {
        if(send(*fd, "Value incremeented\n", strlen("Value incremeented\n"), 0)==-1) {perror("Response failed\n");}
    }
}



int select_str( int* fd, int database){
    if(database == 0 || database == 1){
        char str[strlen("is current string database\n")+4];
        sprintf(str, "%d is current string database\n", database);
        send(*fd, str, strlen(str), 0);
        return database;
    }else{
        send(*fd, "unknown, back to default detabase 1\n", strlen("unknown, back to default detabase 1\n"), 0);
        return 1;
    }
}


void save(int* fd, struct string* list, char* path){
    
    // check if have key-value to save
    if(empty_checker(list)!=1){
        if(send(*fd, "No data to SAVE\n", strlen("No data to SAVE\n"), 0)==-1) {perror("Response failed\n");}
        return ;
    }
    
    // save
    FILE *file;
    file = fopen(path, "w");
    if (file == NULL) {
        perror("open file failed\n");
        if(send(*fd, "Failed to SAVE\n", strlen("Failed to SAVE\n"), 0)==-1) {perror("Response failed\n");}
        return ;
    }
    
    struct string* current_KeyValue = list;
    
    while (current_KeyValue != NULL) {
        
        fprintf(file, "%s %s\n",  (*current_KeyValue).key, (*current_KeyValue).value);
        current_KeyValue = current_KeyValue->next_KeyValue;
    }
    
    fclose(file);
    if(send(*fd, "DONE SAVE\n", strlen("DONE SAVE\n"), 0)==-1) {perror("Response failed\n");}
    
}




// function external sorting
void externalSort(int* fd, const char *dir_path, const char *id) {
    
    //path for input file
    size_t len_path_file = snprintf(NULL, 0, "%s/data_%s", dir_path, id) + 1;
    char file_path[len_path_file];
    snprintf(file_path, len_path_file, "%s/data_%s", dir_path, id);
    
    //path for merged file
    size_t len_path_merge_file = snprintf(NULL, 0, "%s/data_merge_%s", dir_path, id) + 1;
    char merge_file_path[len_path_merge_file];
    snprintf(merge_file_path, len_path_merge_file, "%s/data_merge_%s", dir_path, id);
    
    FILE *input_file = fopen(file_path, "r");
    if (input_file == NULL) {
        printf("open file failed : %s \n", file_path);
        if(send(*fd, "Failed to LOAD\n", strlen("Failed to LOAD\n"), 0)==-1) {perror("Response failed\n");}
        return ;
    }
    
    FILE *sort_files[27];
    char* tab[27] = {"a","b","c","d","e","f","g","h","i","j","k","l","m","n","o","u","p","r","s","t","u","v","w","x","y","z","～"};
    
    //path for sorted files
    for (int i = 0; i < 27; i++) {
        
        size_t lem_sort_files = snprintf(NULL, 0, "%s/%s", dir_path, tab[i]) + 1;
        char sort_files_path[lem_sort_files];
        snprintf(sort_files_path, lem_sort_files, "%s/%s",dir_path, tab[i]);
        
        sort_files[i] = fopen(sort_files_path, "w");
        if (sort_files[i]  == NULL) {
            perror("open file failed\n");
            if(send(*fd, "Failed to LOAD\n", strlen("Failed to LOAD\n"), 0)==-1) {perror("Response failed\n");}
            return ;
        }
    }
    
    // read each line from the input file
    char line[1024];
    memset(line, 0, sizeof(line));
    while (fgets(line, sizeof(line), input_file) != NULL) {
        char first_char = tolower(line[0]);  //if majuscule convert to minuscule
        
        // find the corresponding file based on the first char
        int file_index;
        if ('a' <= first_char && first_char <= 'z' ) {
            file_index = first_char - 'a'; //a to z file
        } else {
            file_index = 26; // ~ file
        }
        
        // write the line to the corresponding output file
        fprintf(sort_files[file_index], "%s", line);
    }
    
    // close all input/output files
    fclose(input_file);
    for (int i = 0; i < 27; i++) {
        fclose(sort_files[i]);
    }
    
    memset(line, 0, sizeof(line));
    
    // merge 27 files back into one file
    FILE *merge_file = fopen(merge_file_path, "w");
    if (merge_file == NULL) {
        perror("open file failed\n");
        if(send(*fd, "Failed to LOAD\n", strlen("Failed to LOAD\n"), 0)==-1) {perror("Response failed\n");}
        return ;
    }
    
    for (int i = 0; i < 27; ++i) {
        
        // open each sorted file
        size_t lem_sort_files = snprintf(NULL, 0, "%s/%s", dir_path,tab[i]) + 1;
        char sort_files_path[lem_sort_files];
        snprintf(sort_files_path, lem_sort_files, "%s/%s",dir_path, tab[i]);
        
        sort_files[i] = fopen(sort_files_path, "r");
        
        // merge
        while (fgets(line, sizeof(line), sort_files[i]) != NULL) {
            fprintf(merge_file, "%s", line);
        }
        fclose(sort_files[i]);
    }
    
    fclose(merge_file);
    if(send(*fd, "DONE SORT\n", strlen("DONE SORT\n"), 0)==-1) {perror("Response failed\n");}
}


void externalGet(int* fd, char* buff, const char *dir_path, const char *id){
    
    // key
    char *targetKey = buff+6;
    targetKey[strcspn(targetKey, "\n")] = '\0';
    printf("target : %s \n",targetKey);
    
    char first_char = tolower(targetKey[0]);
    
    // open the corresponding file based on the first char
    size_t len_path_file = snprintf(NULL, 0, "%s/%c", dir_path, first_char) + 1;
    char file_path[len_path_file];
    snprintf(file_path, len_path_file, "%s/%c", dir_path, first_char);
    FILE *sort = fopen(file_path, "r");
    
    if (sort == NULL) {
        
        printf("Cannot find sorted files\n");
        if(send(*fd, "SORT before FETCH\n", strlen("SORT before FETCH\n"), 0)==-1) {perror("Response failed\n");}
        
    }else{
        
        
        char line[1024];
        memset(line, 0, sizeof(line));
        char* res = NULL;
        
        //searching the target key in the file
        while (fgets(line,sizeof(line),sort) != NULL) {
           
            char *key = strtok(line, " ");
            char *value = strtok(NULL, " ");
            
            if(strcmp(key, targetKey) == 0) {
                res = strdup(value);
                break;
            }
        }
        
        fclose(sort);
        
        if(res == NULL){
            printf("Cannot find value\n");
            if(send(*fd, "Cannot find value in sorted files\n", strlen("Cannot find value in sorted files\n"), 0)==-1) {perror("Response failed\n");}
        }else{
            printf("value : %s",res);
            if(send(*fd, res, strlen(res), 0)==-1) {perror("Response failed\n");}
        }
    }
}



    
