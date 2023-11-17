/**
 * IFJ-IAL Projekt 2023
 * 
 * @file output.h
 * @brief Ukladanie outputu
 * 
 * @author Igor Mikula (xmikul74)
 * @author Marko Olesak (xolesa00)
 * @author Jan Findra (xfindr01)
 * @author Tomas Arlt (xarltt00)
*/

#ifndef OUTPUT_H
#define OUTPUT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct output{
    int size;
    int capacity;
    char **file;
} output_t;

//create new line
char *new_line(char *string);

// inicializacia zoznamu stringov
void init_output(output_t *output);

// vlozenie riadka do "suboru"
void insert_line(output_t *output, char *line);

// vytlacenie "subor"
void print_output(output_t *output);

#endif