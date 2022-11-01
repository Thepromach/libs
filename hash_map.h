
#ifndef HASH_MAP_H
#define HASH_MAP_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define FNV_OFFSET_BASIS 0xcbf29ce484222325
#define FNV_PRIME 0x100000001b3


#ifndef HASH_DEFAULT_ALLOC
    #define HASH_DEFAULT_ALLOC(size) malloc(size)
    #define HASH_DEFAULT_CALLOC(count, size) calloc(count, size)
    #define HASH_DEFAULT_REALLOC(ptr, size) realloc(otr, size)
    #define HASH_DEFAULT_FREE(ptr) free(ptr)
#endif //HASH_DEFAULT_ALLOC


typedef struct HashItem
{
    uint64_t _hash;

    char* key;
    void* value;

} HashItem;



typedef struct HashMap {


    HashItem* items;
    size_t capacity;
    size_t lenght;
    


    void* (*_alloc)(size_t);
    void* (*_calloc)(size_t, size_t);
    void (*_free)(void*);
    void* (*_realloc)(void*, size_t);



} HashMap;


typedef struct HashIterator {

    HashItem* items;
    size_t index;
    size_t* lenght;

} HashIterator;




HashMap* hashmap_new(size_t start_capacity);
void hashmap_destroy(HashMap* map);

void hashmap_add_item(HashMap* map, char* key, void* value);
HashItem* hashmap_find_item(HashMap* map, char* key);
void* hashmap_find_value(HashMap* map, char* key);
void hashmap_clear(HashMap* map);


HashIterator hashmap_iterator(HashMap* map);
size_t hash_iter_next(HashIterator* iterator, HashItem** item);
void hashmap_for_each(HashMap* map, void (*function)(size_t index, HashItem* item));



#endif // HASH_MAP_H
#ifdef HASH_MAP_IMPLEMENTATION



uint64_t fnv_1a (uint8_t* data, size_t lenght){

    uint64_t hash = FNV_OFFSET_BASIS;

    for (size_t i = 0; i < lenght; i++) {

        hash = hash ^ data[i];
        hash = hash * FNV_PRIME;
        

    }

    return hash;

}


HashMap* hashmap_new(size_t start_capacity) {

    HashMap* map = HASH_DEFAULT_ALLOC(sizeof(HashMap));

    map->capacity = start_capacity;
    map->lenght = 0;
    map->_alloc = &HASH_DEFAULT_ALLOC;
    map->_calloc = &HASH_DEFAULT_CALLOC;
    map->_realloc = &HASH_DEFAULT_REALLOC;
    map->_free = &HASH_DEFAULT_FREE;

    if(start_capacity == 0) return map;


    map->items = HASH_DEFAULT_CALLOC(start_capacity, sizeof(HashItem));

    return map;
}


void hashmap_destroy(HashMap* map) {

    for (size_t i = 0; i < map->lenght; i++){

        if ( map->items[i].key != 0) {
            map->_free(map->items[i].value);
        }
    }

    map->_free(map->items);
    map->_free(map);
}

void hashmap_clear(HashMap* map) {

    for (size_t i = 0; i < map->lenght; i++){

        if ( map->items[i].key != 0) {
            map->_free(map->items[i].value);
            map->items[i].key = 0;
            map->items[i]._hash = 0;
        }
    }

    map->lenght = 0;

}


void hashmap_add_item(HashMap* map, char* key, void* value) {

    if (map->capacity == 0) {
        size_t start_capacity = 1;
        map->items = calloc(start_capacity, sizeof(HashItem));
        map->capacity = start_capacity;
    }
    else if (map->lenght >= map->capacity) {
        size_t new_capacity = map->capacity * 2;
        map->items = realloc(map->items, new_capacity * sizeof(HashItem));
        map->capacity = new_capacity;
    }


    uint64_t hash = fnv_1a(key, strlen(key));
    HashItem* item = &map->items[map->lenght];
    map->lenght++;

    item->_hash = hash;
    item->key = key;
    item->value = value;


}




HashItem* hashmap_find_item(HashMap* map, char* key) {

    uint64_t hash = fnv_1a(key, strlen(key));

    for(size_t i = 0; i < map->lenght; i++) {

        if(map->items[i]._hash == hash) {
            return &map->items[i];
        }

    }

    return NULL;
}


void* hashmap_find_value(HashMap* map, char* key) {

    uint64_t hash = fnv_1a(key, strlen(key));

    for(size_t i = 0; i < map->lenght; i++) {

        if(map->items[i]._hash == hash) {
            return map->items[i].value;
        }

    }

    return NULL;
}

HashIterator hashmap_iterator(HashMap* map) {

    HashIterator iterator;
    iterator.index = 0;
    iterator.items = map->items;
    iterator.lenght = &map->lenght;
    return iterator;
}


size_t hash_iter_next(HashIterator* iterator, HashItem** item){

    if (*iterator->lenght <= iterator->index) return 0;

    *item = &iterator->items[iterator->index];

    iterator->index++;


    return 1;
}




void hashmap_for_each(HashMap* map, void (*function)(size_t index, HashItem* item)) {


    for (size_t i = 0; i < map->lenght; i++) {

        function(i, &map->items[i]);


    }


}



#endif // HASH_MAP_IMPLEMENTATION






