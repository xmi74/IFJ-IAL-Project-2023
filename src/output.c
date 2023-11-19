/**
 * IFJ-IAL Projekt 2023
 * 
 * @file output.c
 * @brief Ukladanie outputu
 * 
 * @author Igor Mikula (xmikul74)
 * @author Marko Olesak (xolesa00)
 * @author Jan Findra (xfindr01)
 * @author Tomas Arlt (xarltt00)
*/

#include "output.h"

string_t *new_line(char *string){
    string_t *newLine = malloc(sizeof(string_t));
    dstringInit(newLine);
    int index = 0;
    while (string[index] != '\0'){
        dstringAppend(newLine, string[index++]);
    }
    dstringAppend(newLine, string[index]);
    return newLine;
}

void append_line(string_t *str1, char* str2){
    str1->data[str1->length - 1] = str2[0];
    int index = 1;
    while (str2[index] != '\0'){
        dstringAppend(str1, str2[index++]);
    }
    dstringAppend(str1, str2[index]);
}

void output_init(output_t **output){
    (*output) = malloc(sizeof(output_t));
    (*output)->count = 0;
    (*output)->capacity = 0;
    (*output)->size = 0;
    (*output)->file = NULL;
}

void output_insert_line(output_t *output, string_t *line){
    output->size++;
    if (output->capacity < output->size){
        output->file = realloc(output->file, sizeof(string_t*) * output->size);
        output->capacity++;
    }
    output->file[output->size - 1] = line;
}

void output_print(output_t *output){
    int index = 0;
    while (index < output->size){
        fprintf(stdout, "%s\n", output->file[index]->data);
        dstringFree(output->file[index]);
        free(output->file[index]);
        index++;
    }
    free(output->file);
    free(output);
}