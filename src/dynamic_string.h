/**
 * IFJ-IAL Projekt 2023
 * 
 * @file dynamic_string.h
 * @brief Implementacia struktury a funkcii pre manipulaciu s dynamickym stringom
 * 
 * @author Igor Mikula (xmikul74)
 * @author Marko Olesak (xolesa00)
 * @author Jan Findra (xfindr01)
 * @author
*/

#ifndef _DYNAMIC_STRING_H_
#define _DYNAMIC_STRING_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Struktura dynamickeho retazca
typedef struct {
    char *data;         // Obsah
    size_t length;      // Dlzka retazca
    size_t capacity;    // Kapacita (kolko znakov sa moze ulozit)
} string_t;

// Inicializacia dynamickeho retazca
void dstringInit(string_t *str);

// Uvolnenie pamati dynamickeho retazca
void dstringFree(string_t *str);

// Pridanie znaku na koniec retazca
void dstringAppend(string_t *str, char c);

// Prevedenie celeho retazca zo source -> destination 
// (funguje ako strcpy pre dynamicky retazec)
int dstringCopy(string_t *destination, string_t *source);

// Porovnanie retazcov str_1 a str_2
// (funguje ako strcmp pre dynamicky retazec)
int dstringCompare(string_t *str_1, string_t *str_2);

#endif