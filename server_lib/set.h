#include "string.h"


struct simple_string{
    char* key;
    char* value;
};


struct dic{ // simple set type //student or course
    
    
    char* set_name;
    struct simple_string* set[512]= NULL;
    
};


struct dic_list{ // simple set type //student or course
    
    int size = 512 ;
    int size_mask = size - 1;
    int used = 0;
    struct dic* table[size]= NULL;
    
    
};
