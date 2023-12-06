/**
 * IFJ-IAL Projekt 2023
 * 
 * @file dynamic_string.h
 * @brief Implementacia struktury a funkcii pre manipulaciu s dynamickym stringom
 * 
 * @author Igor Mikula (xmikul74)
 * @author Marko Olesak (xolesa00)
 * @author Jan Findra (xfindr01)
 * @author Tomas Arlt (xarltt00)
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

/**
 * @brief Inicializacia dynamickeho retazca
 * 
 * @param str ukazatel na inicializovany retazec
*/
void dstringInit(string_t *str);

/**
 * @brief Uvolnenie pamati dynamickeho retazca
 * 
 * @param str ukazatel na uvolnovany retazec
*/
void dstringFree(string_t *str);

/**
 * @brief Pridanie znaku na koniec retazca
 * 
 * @param str ukazatel na retazec, do ktoreho sa vklada charakter
 * @param c vkladany charakter
*/
void dstringAppend(string_t *str, char c);

/**
 * @brief Skopirovanie celeho retazca do ineho
 * 
 * @param destination retazec, do ktoreho sa ma vysledok ulozit
 * @param source retazec, ktory ma byt ulozeny do vysledku
*/
int dstringCopy(string_t *destination, string_t *source);

/**
 * @brief Porovnanie retazcov
 * 
 * @param str_1 1. porovnavany retazec
 * @param str_2 2. porovnavany retazec
*/
int dstringCompare(string_t *str_1, string_t *str_2);

#endif
