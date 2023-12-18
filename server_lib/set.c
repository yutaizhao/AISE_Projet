#include "set.h"
#include "murmur3.h"


int hash_find_index(const dic* set_name, const char* element) {
    uint32_t hash[4];                /* Output for the hash */
    uint32_t seed = 520;              /* Seed value for hash, birth day !*/
    uint32_t mask = set_name->size_mask;
    
    MurmurHash3_x64_128(element, strlen(element)+1, seed, hash);
    int index = (hash[0] & mask) + (hash[1] & mask) + (hash[2] & mask) + (hash[3] & mask);
    return index;
}

void SADD(int* fd,char* buff,struct dic_list* list){
    
    char* kv = buff +5;
    char* str = strok(kv,":");
    struct dic* set = (struct dic*)malloc(sizeof(struct dic));
    
    
}
