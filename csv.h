#ifndef CSV_H
#define CSV_H

#include <stdbool.h>

#ifndef CSV_ALLOC
    #define CSV_ALLOC(size) malloc(size)
    #define CSV_REALLOC(ptr, size) realloc(ptr, size)
    #define CSV_FREE(ptr) free(ptr)
#endif //CSV_ALLOC


typedef struct CVS_Table {

    char*** data;

    size_t rows;
    size_t columns;


}CVS_Table;


CVS_Table csv_from_string(char* str);
CVS_Table csv_load_file(const char* filename);


char* csv_to_string(CVS_Table* table);
void csv_write_file(CVS_Table* table);


void csv_free_data(CVS_Table* table);


#endif //CSV_H


#ifdef CSV_H_IMPLEMENTATION

bool __valid_character(char str){
    return (str >= 'A' && str <= 'Z') ||
            (str >= 'a' && str <= 'z') ||
            (str >= '0' && str <= '9') || '"';
}

char* __parse_string(char** str){

    size_t string_size = 0;

    if(str[0][string_size] == '"'){
        str[0]++;
        while(str[0][string_size] != '"'){
            string_size++;
        }
    }
    else {
        while(__valid_character(str[0][string_size])){
            string_size++;
        }
    }
    

    char* result = (char*)CSV_ALLOC(string_size+1);

    strncpy(result, *str, string_size);
    result[string_size] = '\0';
    (*str) += string_size;

    return result;
}

CVS_Table csv_from_string(char* str){

    CVS_Table table;
    table.rows = 1;
    table.columns = 1;

    char* current = str;

    table.data = (char***)CSV_ALLOC(sizeof(char***));
    table.data[0] = (char**)CSV_ALLOC(sizeof(char**));

    size_t current_columns = 0;
    while(current != '\0'){
        char* field = __parse_string(&current);
        table.data[table.rows - 1][current_columns] = field;

        if(str == ',' || str == ';'){
            table.columns++;

            if(current_columns >= table.columns){
                table.columns = current_columns + 1;
                table.data[table.rows - 1] = (char**)CSV_REALLOC(table.data[table.rows - 1], table.columns * sizeof(char**));
            }
        }
        else if(str == '\n')[
            table.rows++;
            current_columns = 0;

            table.data = (char***)CSV_REALLOC(table.data, table.rows * sizeof(char***));
            table.data[0] = (char**)CSV_ALLOC(table.columns * sizeof(char**));
        ]
    }


    return table;
}


CVS_Table csv_load_file(const char* filename) {


    FILE* file = fopen(filename, "r");

    if (file == NULL) {

        CVS_Table table;
        table.rows = 0;
        table.columns = 0;
        table.data = NULL;
        return table;

    };

    fseek(file, 0L, SEEK_END);
    size_t size = ftell(file);
    fseek(file, 0, SEEK_SET);

    
    char* file_string = (char*)CSV_ALLOC(size + 1);
    fread(file_string, size, 1, file);
    fclose(file);

    
    return csv_from_string(&file_string);
}

#endif //CSV_H_IMPLEMENTATION