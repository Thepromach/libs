
#ifndef ARGS_PARSER_H
#define ARGS_PARSER_H

#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#ifndef ARGS_ALLOC
    #define ARGS_ALLOC malloc
    #define ARGS_CALLOC calloc
    #define ARGS_REALLOC realloc
    #define ARGS_FREE free
#endif //ARGS_ALLOC

typedef union Result {

    char* string;
    long int integer;
    float float_number;

} Result;


typedef struct Argument {
    char* name;
    char* arg_type;
    Result results[8];

    bool was_set;
} Argument;

typedef struct ArgsParser {

    size_t argument_count;
    Argument* arguments; 


} ArgsParser;


ArgsParser args_parser_new();
void args_parser_add_argument(ArgsParser* parser, const char* name);
void args_parser_parse(ArgsParser* parser, int argc, char* argv[]);

void args_parser_for_each(ArgsParser* parser, void (*function)(const char* name, const char* arg_types, Result result[]));


void args_parser_free(ArgsParser* parser);



#endif //ARGS_PARSER_H


#ifdef ARGS_PARSER_IMPLEMENTATION

ArgsParser args_parser_new() {

    ArgsParser parser;
    parser.argument_count = 0;
    parser.arguments = NULL;

    return parser;
}

void args_parser_add_argument(ArgsParser* parser, const char* name) {

    if (parser->argument_count == 0) {

        parser->arguments = ARGS_ALLOC(sizeof(Argument));
    }
    else { parser->arguments = ARGS_REALLOC(parser->arguments, sizeof(Argument) * (parser->argument_count + 1)); }


    size_t name_lenght = strlen(name);


    size_t i;
    for (i = 0; i < name_lenght; i++) {
        if(name[i] == ' ') break;
    }

    parser->arguments[parser->argument_count].name = ARGS_ALLOC(i + 1);
    strncpy(parser->arguments[parser->argument_count].name, name, i);
    parser->arguments[parser->argument_count].name[i] = '\0';
    parser->arguments[parser->argument_count].was_set = false;

    

    if(name_lenght <= i) {
        
        parser->arguments[parser->argument_count].arg_type = NULL;
        parser->argument_count++;
        return;
    }
    

    parser->arguments[parser->argument_count].arg_type = ARGS_CALLOC(8, 1);


    for (size_t j = 0; i < name_lenght || j >= 8; i++){
        if(name[i] == '%'){
            
            i++;
            
            if(name[i] == 's' || name[i] == 'i' || name[i] == 'f') {
                
                parser->arguments[parser->argument_count].arg_type[j] = name[i];
                
                j++;
            }
        }
    }

    
    parser->argument_count++;

}

void args_parser_parse(ArgsParser* parser, int argc, char* argv[]) {


    for (size_t i = 0; i < parser->argument_count; i++) {
        
        for (size_t j = 0; j < argc; j++) {
            if(strcmp(parser->arguments[i].name, argv[j]) == 0){
                parser->arguments[i].was_set = true;
                if(parser->arguments[i].arg_type != NULL && (j+1) < argc) {
                    j++;
                    
                    for(size_t w = 0; w < strlen(parser->arguments[i].arg_type); w++){
                        if(j >= argc) {
                            parser->arguments[i].results[w].string = NULL;
                        }
                        else if (parser->arguments[i].arg_type[w] == 's') {   
                            parser->arguments[i].results[w].string = argv[j];
                        }
                        else if (parser->arguments[i].arg_type[w] == 'i') {
                             parser->arguments[i].results[w].integer = strtol(argv[j], NULL, 10);
                        }
                        else if (parser->arguments[i].arg_type[w] == 'f') {
                             parser->arguments[i].results[w].float_number = strtof (argv[j], NULL);
                        }
                        j++;

                        
                        
                    }
                } else {
                    parser->arguments[i].results[0].integer = 0;
                }
            }
        }
    }
}

void args_parser_for_each(ArgsParser* parser, void (*function)(const char* name, const char* arg_types, Result result[])) {

    for (size_t i = 0; i < parser->argument_count; i++) {

        

        if(parser->arguments[i].was_set) function(parser->arguments[i].name, parser->arguments[i].arg_type, parser->arguments[i].results);

    }

}


void args_parser_free(ArgsParser* parser){


    for (size_t i = 0; i < parser->argument_count; i++) {
        ARGS_FREE(parser->arguments[i].name);

        if(parser->arguments->arg_type != NULL){
            ARGS_FREE(parser->arguments[i].name);
        }


    }

    ARGS_FREE(parser->arguments);


}

#endif //ARGS_PARSER_IMPLOMENTATION