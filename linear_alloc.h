#ifndef LINEAR_ALLOC_H
#define LINEAR_ALLOC_H

void linear_init();
void* linear_alloc(size_t size);
void linear_reset();
void linera_end();


#ifndef LINEAR_ALLOC_MEMORY_BLOCK
#define LINEAR_ALLOC_MEMORY_BLOCK (1024 * 1024)
#endif //LINEAR_ALLOC_MEMORY_BLOCK


#define linear_new(type) (type)linear_alloc(sizeof(type)) 


#endif //LINEAR_ALLOC_H


#ifdef LINEAR_ALLOC_H_IMPLEMENTATION

#ifdef _WIN32 
    #include <Memoryapi.h>
#else
    #include <stdlib.h>
#endif //_WIN32


void* __linear_memory;
void* __linear_current;
void* __linear_end;

void linear_init() {
    #ifdef _WIN32 
        __linear_memory = (void*) VirtualAlloc(NULL, POOL_TOTAL_SIZE, MEM_COMMIT, PAGE_READWRITE)
    #else
        __linear_memory = malloc(POOL_TOTAL_SIZE);
    #endif //_WIN32 

    __linear_current = __linear_memory;
    __linear_end = __linear_memory + POOL_TOTAL_SIZE;

}


void* linear_alloc(size_t size) {

    if ((size + __linear_current) >= __linear_end)
        return NULL;

    
    void* result = __linear_current;
    __linear_current += size;

    return result;
}   


void linear_reset() {

    __linear_current = __linear_memory;

}


void linear_end() {
    #ifdef _WIN32 
       VirtualFree(__linear_memory,0, MEM_RELEASE);
    #else
        free(__linear_memory);
    #endif //_WIN32 
}

#endif //LINEAR_ALLOC_H_IMPLEMENTATION
