/**
 * IFJ-IAL Projekt 2023
 *
 * @file stack.h
 * @brief Implementacia zasobnika tokenov
 *
 * @author Igor Mikula (xmikul74)
 * @author Marko Olesak (xolesa00)
 * @author Jan Findra (xfindr01)
 * @author Tomas Arlt (xarltt00)
 */


#ifndef STACK_H
#define STACK_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "scanner.h"

// Definicie konstant pre inicializacnu kapacitu zasobnika a faktor prealokacie resp. dealokacie
#define STACK_INITIAL_CAPACITY 10
#define STACK_REALLOC_FACTOR 0.8
#define STACK_DEALLOC_FACTOR 0.3

// Definicia struktury pre prvok zasobnika
typedef struct
{
    int topIndex;      // Index prvku na vrchole zasobnika
    size_t capacity;   // Aktualna kapacita zasobnika
    size_t size;       // Aktualny pocet prvkov v zasobniku
    token_t *elements; // Pole prvkov zasobnika
} Stack;

/**
 * @brief Inicializacia zasobnika
 * @param stack Ukazatel na zasobnik
 * @return void
*/
void Stack_Init(Stack *stack);

/**
 * @brief Kontrola a zmena velkosti zasobnika, ak je to potrebne
 * @param stack Ukazatel na zasobnik
 * @return void
*/
void Stack_CheckSize(Stack *stack);

/**
 * @brief Funkcia na zistenie, ci je zasobnik prazdny
 * Funkcia vrati true, ak je velkost zasobnika 0, inak false
 * @param stack Ukazatel na zasobnik
 * @return bool
*/
bool Stack_IsEmpty(const Stack *stack);

/**
 * @brief Funkcia na zistenie prvku na vrchole zasobnika
 * Funkcia nahra do dataPtr adresu prvku na vrchole zasobnika
 * @param stack Ukazatel na zasobnik
*/
void Stack_Top(const Stack *stack, token_t *dataPtr);

/**
 * @brief Funkcia na odstranenie prvku z vrcholu zasobnika
 * @param stack Ukazatel na zasobnik
 * @return void
*/
void Stack_Pop(Stack *stack);

/**
 * @brief Funkcia na vlozenie prvku na vrchol zasobnika
 * @param stack Ukazatel na zasobnik
 * @param element Ukazatel na pridavany prvok
 * @return void
*/
void Stack_Push(Stack *stack, token_t *element);

/**
 * @brief Funkcia na uvolnenie struktury
 * @param stack Ukazatel na zasobnik
 * @return void
*/
void Stack_Dispose(Stack *stack);

/**
 * @brief Funkcia vracia najvyssi token, ktory je terminal
 * @param stack Ukazatel na zasobnik
 * @return token_t
*/
token_t *Stack_GetTopTerminal(Stack *stack);

/**
 * @brief Funkcia sluzi na vlozenie '<' pred prvy terminal v zasobniku.
 * Vyhlada index vlozenia, posunie vsetky prvky o jedno miesto doprava a vlozi '<'.
 * @param stack Ukazatel na zasobnik
 * @return void
*/
void Stack_InsertLesser(Stack *stack);

/**
 * @brief Funkcia sluzi na odstranenie vsetkych prvkov z vrcholu zasobnika az po najblizsi terminal.
 * @param stack Ukazatel na zasobnik
 * @return void
*/
void Stack_PopUntilLesser(Stack *stack);

#endif