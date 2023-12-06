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

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "scanner.h"

#ifndef STACK_H
#define STACK_H

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

void Stack_Init(Stack *stack);

void Stack_CheckSize(Stack *stack);

bool Stack_IsEmpty(const Stack *stack);

void Stack_Top(const Stack *stack, token_t *dataPtr);

void Stack_Pop(Stack *stack);

void Stack_Push(Stack *stack, token_t *element);

void Stack_Dispose(Stack *stack);

void Stack_Print(Stack *stack);

token_t *Stack_GetTopTerminal(Stack *stack);

void Stack_InsertLesser(Stack *stack);

void Stack_PopUntilLesser(Stack *stack);

#endif