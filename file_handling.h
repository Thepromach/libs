
#ifndef FILE_HANDLING_H
#define FILE_HANDLING_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <sys/stat.h>

char* load_file_string(const char* filename);
char** load_file_lines(const char* filename, size_t* lines);
uint8_t* load_file_bytes(const char* filename, size_t* size);

size_t write_file_string(const char* filename, char* file_string);
size_t write_file_lines(const char* filename, char** file_string, size_t lines);
size_t write_file_bytes(const char* filename, uint8_t bytes, size_t size);

size_t file_append(const char* filename, char* text);

size_t file_exist(const char* filename);

#endif // FILE_HANDLING_H
#ifdef FILE_HANDLING_H_IMPLEMENTATION

char* load_file_string(const char* filename) {

    FILE* file = fopen(filename, "r");

    if (file == NULL) return NULL;

    fseek(file, 0L, SEEK_END);
    size_t size = ftell(file);
    fseek(file, 0, SEEK_SET);

    
    char* file_string = (char*)calloc(size + 1, 1);
    fread(file_string, size, 1, file);
    fclose(file);

    return file_string;
}


char** load_file_lines(const char* filename, size_t* line_count) {

    FILE* file = fopen(filename, "r");
    if (file == NULL) return NULL;

    *line_count = 0;
    char** lines = (char**)calloc(1, sizeof(size_t));

    #define FILE_BUFFER_SIZE 1024
    char buffer[FILE_BUFFER_SIZE];

    while(fgets(buffer, FILE_BUFFER_SIZE, file) != NULL) {

        lines[*line_count] = (char*) malloc(strlen(buffer));
        strcpy(lines[*line_count], buffer);
        *line_count++;

        lines = (char**) realloc(lines, (*line_count + 1) * sizeof(size_t));
        lines[*line_count] = NULL; 

    }

    return lines;

}


uint8_t* load_file_bytes(const char* filename, size_t* size) {

    FILE* file = fopen(filename, "r");

    if (file == NULL) return NULL;

    fseek(file, 0L, SEEK_END);
    *size = ftell(file);
    fseek(file, 0, SEEK_SET);

    
    uint8_t* file_data = (uint8_t*)calloc(*size, 1);
    fread(file_data, *size, 1, file);
    fclose(file);

    return file_data;
}




size_t write_file_string(const char* filename, char* file_string) {

    FILE* file = fopen(filename, "w");
    if (file == NULL) return 0;

    fprintf(file, "%s", file_string);

    fclose(file);
    return 1;
}




size_t write_file_lines(const char* filename, char** file_string, size_t lines)
{

    FILE* file = fopen(filename, "w");
    if (file == NULL) return 0;

    for(size_t i = 0; i < lines; i++){
        fprintf(file, "%s\n", file_string[i]);
    }

    fclose(file);
    return 1;
}



size_t write_file_bytes(const char* filename, uint8_t bytes, size_t size) {

    FILE* file = fopen(filename, "w");
    if (file == NULL) return 0;

    fwrite((void*)bytes, 1, size, file);

    fclose(file);
    return 1;
}


size_t file_append(const char* filename, char* text) {

    FILE* file = fopen(filename, "a");
    if (file == NULL) return 0;

    fprintf(file, "%s", text);

    fclose(file);
    return 1;
}



size_t file_exist(const char* filename) {

    struct stat buffer;
    int exist = stat(filename,&buffer);
    if(exist == 0) return 1;
         
    return 0;
}


#endif //FILE_HANDLING_H_IMPLEMENTATION 



