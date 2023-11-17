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

char *new_line(char *string){
    char *newLine = malloc(strlen(string) + 1);
    strcpy(newLine, string);
    return newLine;
}

void init_output(output_t *output){
    output->capacity = 0;
    output->size = 0;
    output->file = NULL;
}

void insert_line(output_t *output, char *line){
    output->size++;
    if (output->capacity < output->size){
        output->file = realloc(output->file, sizeof(char*) * output->size);
        output->capacity++;
    }
    char *newLine = new_line(line);
    output->file[output->size - 1] = newLine;
}

void print_output(output_t *output){
    while (output->size-- > 0){
        fprintf(stdout, "%s\n", output->file[output->size]);
        free(output->file[output->size]);
    }
    free(output->file);
}