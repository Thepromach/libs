

#ifndef JSON_H
#define JSON_H


#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdbool.h>

#ifndef JSON_ALLOC
    #define JSON_ALLOC(size) malloc(size)
    #define JSON_REALLOC(ptr, size) realloc(ptr, size)
    #define JSON_FREE(ptr) free(ptr)
#endif //JSON_ALLOC


typedef enum JSON_Type {

    JSON_UNKNOWN,
    JSON_OBJECT,
    JSON_ARRAY,
    JSON_STRING,
    JSON_NUMBER,
    JSON_BOOLEAN,
    JSON_NULL,

} JSON_Type;

typedef struct JSON_Object_Header {

    size_t member_count;
    char** member_names;
    struct JSON_Object* members;

} JSON_Object_Header;


typedef struct JSON_Object {

    union {
        JSON_Object_Header obj_header;
        char* string;
        double number;
        bool boolean;
    };

    JSON_Type type;


} JSON_Object;


JSON_Object json_load_file(const char* filename);
JSON_Object json_from_string(char* json_string);


char* json_to_string(JSON_Object* obj);
bool json_write_file(JSON_Object* obj, const char* filename);


JSON_Object json_copy(JSON_Object obj);
void json_add_member(JSON_Object* obj, const char* name, JSON_Object member);
JSON_Object* json_get_member(JSON_Object* obj, const char* name);
void json_free_members(JSON_Object* obj);

#endif //JSON_H



#ifdef JSON_H_IMPLEMENTATION




//
//  JSON from string
//

void __eat_whitespaces(char** string){

    while(**string == ' ' || **string == '\n' || **string == '\t'){
        (*string)++;
    }

}


char __get_next_token(char** json_string) {
    __eat_whitespaces(json_string);
    return **json_string;
}



char* __json_parse_string(char** json_string){

    __eat_whitespaces(json_string);
    if(**json_string != '"') return NULL;

    (*json_string)++;

    size_t string_size = 0;
    while(json_string[0][string_size] != '"'){
        if(json_string[0][string_size] == '\0') return NULL;
        string_size++;
    }

    char* result = (char*)JSON_ALLOC(string_size+1);
    strncpy(result, *json_string, string_size);
    result[string_size] = '\0';
    (*json_string) += string_size+1;


    return result;
}



double __json_parse_number(char** json_string) {
    double result = strtod(*json_string, json_string);
    return result;
}

JSON_Object __json_from_string(char** json_string){
    JSON_Object result; 

    char token = __get_next_token(json_string);

    if(token == '{') {
        (*json_string)++;
        result.type = JSON_OBJECT;
        result.obj_header.member_names = NULL;
        result.obj_header.members = NULL;
        result.obj_header.member_count = 0;


        while(__get_next_token(json_string) != '}') {

            

            result.obj_header.member_count++;
            result.obj_header.member_names = (char**)JSON_REALLOC(result.obj_header.member_names, result.obj_header.member_count * sizeof(char*));
            result.obj_header.member_names[result.obj_header.member_count - 1] = __json_parse_string(json_string);
            
            

            if(__get_next_token(json_string) != ':') break;
            (*json_string)++;
            

            result.obj_header.members = (JSON_Object*)JSON_REALLOC(result.obj_header.members, result.obj_header.member_count * sizeof(JSON_Object));
            result.obj_header.members[result.obj_header.member_count - 1] = __json_from_string(json_string);
            
            if(__get_next_token(json_string) != ',') break;
            (*json_string)++;
        }
        (*json_string)++;

        
    }
    else if(token == '"'){
        result.type = JSON_STRING;
        result.string = __json_parse_string(json_string);
    }
    else if(token >= '0' && token <= '9' || token == '-'){
        result.type = JSON_NUMBER;
        
        result.number = __json_parse_number(json_string);

        
    }
    else if (token == '[') {
        (*json_string)++;

        result.type = JSON_ARRAY;
        result.obj_header.member_names = NULL;
        result.obj_header.members = NULL;
        result.obj_header.member_count = 0;

        while(__get_next_token(json_string) != ']') {
            
            result.obj_header.member_count++;
            result.obj_header.members = (JSON_Object*)JSON_REALLOC(result.obj_header.members, result.obj_header.member_count * sizeof(JSON_Object));
            result.obj_header.members[result.obj_header.member_count - 1] = __json_from_string(json_string);

            if(__get_next_token(json_string) != ',') break;
            (*json_string)++;
        }
        (*json_string)++;
    }
    else if (strncmp((*json_string), "true", 4) == 0){
        result.type = JSON_BOOLEAN;
        result.boolean = true;
        (*json_string) += 4;
    }
    else if (strncmp((*json_string), "false", 5) == 0){
        result.type = JSON_BOOLEAN;
        result.boolean = false;
        (*json_string) += 5;
    }
    else if (strncmp((*json_string), "null", 4) == 0){
        result.type = JSON_NULL;
        (*json_string) += 4;
    }
    else {
        result.type = JSON_UNKNOWN;
    }
    return result;
}

JSON_Object json_from_string(char* json_string) {

    return __json_from_string(&json_string);

}


JSON_Object json_load_file(const char* filename) {


    FILE* file = fopen(filename, "r");

    if (file == NULL) {

        JSON_Object obj;
        obj.type = JSON_UNKNOWN;
        return obj;

    };

    fseek(file, 0L, SEEK_END);
    size_t size = ftell(file);
    fseek(file, 0, SEEK_SET);

    
    char* file_string = (char*)JSON_ALLOC(size + 1);
    fread(file_string, size, 1, file);
    fclose(file);

    
    return __json_from_string(&file_string);
}



//
//  JSON to string/file
//

void __print_to_result(char** result, size_t* result_size, char* format, ...) {

    va_list args;
    va_start(args, format);

    static char buffer[64];
    size_t count = vsprintf(buffer, format, args);

    (*result) = JSON_REALLOC((*result), (*result_size) + count + 1);
    strncpy((*result) + (*result_size), buffer, count);
    (*result_size) += count;

    va_end(args);

}

void __print_padding(int padding, char** result, size_t* result_size){
    for(int i = 0; i < padding; i++){
        __print_to_result(result, result_size, " ");
    }
}

void __json_to_string(JSON_Object* obj, char** result, size_t* result_size, int padding) {
    if(obj->type == JSON_OBJECT){

        __print_to_result(result, result_size, "{\n");

        padding += 4;
        for(size_t i = 0; i < obj->obj_header.member_count; i++){

            __print_padding(padding, result, result_size);

            __print_to_result(result, result_size, "\"%s\" : ", obj->obj_header.member_names[i]);
            __json_to_string(&obj->obj_header.members[i], result, result_size, padding);
            if(i != (obj->obj_header.member_count-1) ) __print_to_result(result, result_size, ",");
            __print_to_result(result, result_size, "\n");
        }

        __print_padding(padding - 4, result, result_size);
        __print_to_result(result, result_size, "}");
    }
    else if(obj->type == JSON_ARRAY) {

        __print_to_result(result, result_size, "[\n");

        padding += 4;
        for(size_t i = 0; i < obj->obj_header.member_count; i++){

            __print_padding(padding, result, result_size);
            __json_to_string(&obj->obj_header.members[i], result, result_size, padding+1);
            if(i != (obj->obj_header.member_count-1) ) __print_to_result(result, result_size, ",");
            __print_to_result(result, result_size, "\n");
        }

        __print_padding(padding - 4, result, result_size);
        __print_to_result(result, result_size, "]");
    }
    else if(obj->type == JSON_STRING) {
        __print_to_result(result, result_size, "\"%s\"", obj->string);
    }
    else if(obj->type == JSON_NUMBER) {
        __print_to_result(result, result_size, "%lf", obj->number);
    }
    else if(obj->type == JSON_BOOLEAN) {
        if(obj->boolean) __print_to_result(result, result_size, "true");
        else __print_to_result(result, result_size, "false");
    }
    else if(obj->type == JSON_NULL) {
        __print_to_result(result, result_size, "null");
    }
}


char* json_to_string(JSON_Object* obj) {

    size_t result_size = 0;
    char* result = NULL;
    __json_to_string(obj, &result, &result_size, 0);
    
    if(result != NULL)
        result[result_size] = '\0';
    
    return result;
}

bool json_write_file(JSON_Object* obj, const char* filename) {

    FILE* file = fopen(filename, "w");

    if (file == NULL) {

        return false;

    };

    size_t result_size = 0;
    char* result = NULL;
    __json_to_string(obj, &result, &result_size, 0);
    
    if(result == NULL)
        return false;

    result[result_size] = '\0';
    fprintf(file, result);

    fclose(file);

}



//
//  Other functions
//

JSON_Object json_copy(JSON_Object obj){

    if(obj.type == JSON_OBJECT){

        JSON_Object copy;

        copy.type = JSON_OBJECT;
        copy.obj_header.member_count = obj.obj_header.member_count;
        copy.obj_header.member_names = JSON_ALLOC(copy.obj_header.member_count * sizeof(char*));
        copy.obj_header.members = JSON_ALLOC(copy.obj_header.member_count * sizeof(JSON_Object));

        for(size_t i = 0; i < copy.obj_header.member_count; i++){

            //Copy the name
            size_t str_len = strlen(obj.obj_header.member_names[i]);
            copy.obj_header.member_names[i] = JSON_ALLOC(str_len  + 1);
            strncpy(copy.obj_header.member_names[i], obj.obj_header.member_names[i], str_len);
            copy.obj_header.member_names[i][str_len] = '\0';


            copy.obj_header.members[i] = json_copy(obj.obj_header.members[i]);

        }

        return copy;

    }
    else if(obj.type == JSON_ARRAY){

        JSON_Object copy;

        copy.type = JSON_ARRAY;
        copy.obj_header.member_count = obj.obj_header.member_count;
        copy.obj_header.member_names = NULL;
        copy.obj_header.members = JSON_ALLOC(copy.obj_header.member_count * sizeof(JSON_Object));

        for(size_t i = 0; i < copy.obj_header.member_count; i++){
            copy.obj_header.members[i] = json_copy(obj.obj_header.members[i]);
        }

        return copy;
    }
    else if (obj.type == JSON_STRING){
        JSON_Object copy;
        copy.type = JSON_STRING;

        size_t str_len = strlen(obj.string);
        copy.string = JSON_ALLOC(str_len  + 1);
        strncpy(copy.string, obj.string, str_len);
        copy.string[str_len] = '\0';
        return copy;
    }
    

    return obj;
}


void json_add_member(JSON_Object* obj, const char* name, JSON_Object new_member){

    if(name == NULL) return;

    JSON_Object copy = json_copy(new_member);

    obj->obj_header.member_count++;
    obj->obj_header.member_names = JSON_REALLOC(obj->obj_header.member_names, obj->obj_header.member_count * sizeof(char*));
    obj->obj_header.members = JSON_REALLOC(obj->obj_header.members, obj->obj_header.member_count * sizeof(JSON_Object));

    size_t name_lenght = strlen(name);
    char* name_copy = JSON_ALLOC(name_lenght+1);
    strncpy(name_copy, name, name_lenght);
    name_copy[name_lenght] = '\0';

    obj->obj_header.member_names[obj->obj_header.member_count - 1] = name_copy;

    

    obj->obj_header.members[obj->obj_header.member_count - 1] = copy;

}


JSON_Object* json_get_member(JSON_Object* obj, const char* name) {

    if(obj->type != JSON_OBJECT) return NULL;

    for(size_t i = 0; i <obj->obj_header.member_count; i++){

        if(strcmp(obj->obj_header.member_names[i], name) == 0) {
            return &obj->obj_header.members[i];
        }

    }
    return NULL;
}


void json_free_members(JSON_Object* obj){

    if(obj->type == JSON_OBJECT) {
        for(size_t i = 0; i < obj->obj_header.member_count; i++){
            json_free_members(&obj->obj_header.members[i]);
            JSON_FREE(obj->obj_header.member_names[i]);
        }

        JSON_FREE(obj->obj_header.members);
        JSON_FREE(obj->obj_header.member_names);

        obj->obj_header.member_count = 0;
        obj->obj_header.members = NULL;
        obj->obj_header.member_names = NULL;

    }
    else if(obj->type == JSON_ARRAY) {
        for(size_t i = 0; i < obj->obj_header.member_count; i++){
            json_free_members(&obj->obj_header.members[i]);
        }
        JSON_FREE(obj->obj_header.members);

        obj->obj_header.member_count = 0;
        obj->obj_header.members = NULL;

    }

}


#endif //JSON_H_IMPLOMENTATION 