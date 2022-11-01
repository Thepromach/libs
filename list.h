
#ifndef LIST_H
#define LIST_H

#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#ifndef LIST_DEFAULT_ALLOC
    #define LIST_DEFAULT_ALLOC(size) malloc(size)
    #define LIST_DEFAULT_CALLOC(count, size) calloc(count, size)
    #define LIST_DEFAULT_REALLOC(ptr, size) realloc(ptr, size)
    #define LIST_DEFAULT_FREE(ptr) free(ptr)
#endif //LIST_DEFAULT_ALLOC


typedef struct List{

    void** _pointers;
    void* _data;

    size_t capacity;        //this is in terms of elements (so true size of the _data is capacity * element_size and _pointers is capacity * sizeof(size_t))
    size_t element_size; 

    size_t lenght;

    void* (*_alloc)(size_t);
    void* (*_calloc)(size_t, size_t);
    void (*_free)(void*);
    void* (*_realloc)(void*, size_t);


} List;

typedef struct ListIterator{

    List* list;
    
    size_t _index;


} ListIterator;


#define LIST_NEW(type, capasity) list_new(sizeof(type), captasity)

List* list_new(size_t element_size);

size_t list_push(List* list, void* value);
size_t list_push_end(List* list, void* value);
void list_pop(List* list);
void* list_get(List* list, size_t index);
void list_delete(List* list, size_t index);

ListIterator list_iterator(List* list);
size_t list_iter_next(ListIterator* iter, void** value);

void list_for_each(List* list, void (*function)(size_t index, void* value));

void list_clear(List* list);
void list_destroy(List* list);

#endif //LIST_H

#ifdef LIST_H_IMPLEMENTATION 

List* list_new(size_t element_size, size_t capasity) {

    List* list = (List*) LIST_DEFAULT_ALLOC(sizeof(List));
    list->element_size = element_size;
    list->capacity = capasity;
    list->lenght = 0;
    

    list->_alloc = &LIST_DEFAULT_ALLOC;
    list->_calloc = &LIST_DEFAULT_CALLOC;
    list->_realloc = &LIST_DEFAULT_REALLOC;
    list->_free = &LIST_DEFAULT_FREE;

    if(capasity == 0) return list;


    list->_data = LIST_DEFAULT_CALLOC(capasity, element_size);
    list->_pointers = (void**)LIST_DEFAULT_CALLOC(capasity, sizeof(size_t));


    return list;

}


inline void _list_expand_if_need(List* list) {
    if (list->capacity == 0) {
        size_t start_capacity = 1;
        list->_data = list->_calloc(start_capacity, list->element_size);
        list->_pointers = (void**) list->_calloc(start_capacity, sizeof(size_t));
        list->capacity = start_capacity;
    }
    else if(list->lenght >= list->capacity) {
        size_t new_capacity = list->capacity * 2;
        list->_data = list->_realloc(list->_data, new_capacity * list->element_size);
        list->_pointers = (void**) list->_realloc(list->_pointers, new_capacity * sizeof(size_t));
        memset(list->_pointers + list->capacity * sizeof(size_t), NULL, list->capacity * sizeof(size_t));

        list->capacity = new_capacity;
    }
}

size_t list_push(List* list, void* value) {

    _list_expand_if_need(list);

    for (size_t i = 0; i < list->capacity; i++){

        if(list->_pointers[i] == NULL) {
            void* ptr = ((uint8_t*)list->_data + i * list->element_size);
            memcpy(ptr, value, list->element_size);
            list->_pointers[i] = ptr;
            list->lenght++;
            return i;
        }

    }


    return 0; //MAYBE RETURN SOMETHING ELSE? 
}


size_t list_push_end(List* list, void* value) {

    _list_expand_if_need(list);

    void* ptr = ((uint8_t*)list->_data + list->lenght * list->element_size);
    memcpy(ptr, value, list->element_size);
    list->_pointers[list->lenght] = ptr;
    list->lenght++;
    return list->lenght-1;

}


void list_pop(List* list) {

    list->lenght--;
    list->_pointers[list->lenght] = NULL;

}


void* list_get(List* list, size_t index) {

    for (size_t i = 0; list->capacity; i++){

        if(list->_pointers[i] != NULL) list->_pointers[i];
    }



    return NULL;
}


void list_delete(List* list, size_t index) {

    for (size_t i = 0; list->capacity; i++){

        if(list->_pointers[i] != 0) index--;

        if(index == 0) list->_pointers[i] == NULL;
    }

}



ListIterator list_iterator(List* list) {

    ListIterator iterator;
    iterator.list = list;
    iterator._index = 0;

}


size_t list_iter_next(ListIterator* iter, void** value) {

    List* list = iter->list;
    for (size_t i = iter->_index; i < list->capacity; i++){

        if(list->_pointers[i] != NULL) {

            *value = list->_pointers[i];
            iter->_index = i + 1;
            return 1;
        }

    }
    
    return 0;

}


void list_for_each(List* list, void (*function)(size_t index, void* value)){

    size_t index = 0;

    for(size_t i = 0; i < list->capacity; i++){

        if(list->_pointers[i] != NULL){
            function(index, list->_pointers[i]);
            index++;
        }

    }
}


void list_clear(List* list) {

    memset(list->_pointers, NULL, list->capacity * sizeof(size_t));
    list->lenght = 0;
}


void list_destroy(List* list) {

    list->_free(list->_pointers);
    list->_free(list->_data);

    list->_free(list);

}

#endif //LIST_H_IMPLEMENTATION

