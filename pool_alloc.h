
#ifndef POOL_ALLOC_H
#define POOL_ALLOC_H

struct Chunk {

    struct Chuck* next;

};



void pool_init();
void* pool_alloc_ex(size_t size, struct Chuck** pool_head);
void* pool_alloc(size_t size);
void pool_free(void* ptr);
void pool_free_ex(void* ptr, struct Chuck** pool_head);
void pool_end();




#ifndef POOL_COUNT
    #define POOL_COUNT 1024
#endif //POOL_COUNT

#ifndef POOL_SIZE
    #define POOL_SIZE 1024
#endif //POOL_SIZE



#define POOL_TOTAL_SIZE (POOL_COUNT * POOL_SIZE)
#define pool_new(type) (type)pool_alloc(sizeof(type))



#endif //POOL_ALLOC_H



#ifdef POOL_ALLOC_H_IMPLEMENTATION

#ifdef _WIN32 
    #include <Memoryapi.h>
#else
    #include <stdlib.h>
#endif //_WIN32

void* __pool_memory;
struct Chuck* __pool_head;

void pool_init() {

    #ifdef _WIN32 
        __pool_memory = (void*) VirtualAlloc(NULL, POOL_TOTAL_SIZE, MEM_COMMIT, PAGE_READWRITE)
    #else
        __pool_memory = malloc(POOL_TOTAL_SIZE);
    #endif //_WIN32 


    /*head = (struct Chuck*)memory;
    struct Chuck* current = head;*/

    struct Chuck* current = __pool_memory + POOL_TOTAL_SIZE - POOL_SIZE;
    current->next NULL;

    while(current != __pool_memory) {
        
        struct Chuck* next = current - POOL_SIZE;
        next->next = current;
        current = next;

    }
    __pool_head = __pool_memory;

}


void* pool_alloc_ex(size_t size, struct Chuck** pool_head){

    if(size >= POOL_SIZE) return NULL;

    void* result = (void*)(*pool_head);
    (*pool_head) = pool_head->next;

    return result;
}

void* pool_alloc(size_t size){

    if(size >= POOL_SIZE) return NULL;

    void* result = (void*)__pool_head;
    __pool_head = __pool_head->next;

    return result;
}


void pool_free_ex(void* ptr, struct Chuck** pool_head){

    struct Chuck new_head = ptr;
    ptr->next = (*pool_head);
    (*pool_head) = new_head;
}

void pool_free(void* ptr){

    struct Chuck new_head = ptr;
    ptr->next = __pool_head;
    __pool_head = new_head;
}


void pool_end() {
    #ifdef _WIN32 
       VirtualFree(__pool_memory,0, MEM_RELEASE);
    #else
        free(__pool_memory);
    #endif //_WIN32 
}

#endif //POOL_ALLOC_H_IMPLEMENTATION