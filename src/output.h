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
#include "dynamic_string.h"


typedef struct output{
    int size;
    int capacity;
    string_t **file;
} output_t;

//create new line
string_t *new_line(char *string);

// inicializacia zoznamu stringov
void output_init(output_t **output);

// vlozenie riadka do "suboru"
void output_insert_line(output_t *output, string_t *line);

// vytlacenie "subor"
void output_print(output_t *output);

#endif