/**
 * IFJ-IAL Projekt 2023
 *
 * @file stack.c
 * @brief Implementacia dynamickeho ADT stack
 *
 * @author Igor Mikula (xmikul74)
 * @author Marko Olesak (xolesa00)
 * @author Jan Findra (xfindr01)
 * @author Tomas Artl (xarltt00)
 */

// TODO : error handling, viac testov ?

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "stack.h"

// Funkcia na inicializaciu zasobnika
void Stack_Init(Stack *stack)
{
    // Kontrola, ci bola struktura spravne inicializovana
    if (stack == NULL)
    {
        // Chyba
        return;
    }

    // Inicializacia vlastnosti zasobnika
    stack->topIndex = -1;
    stack->capacity = STACK_INITIAL_CAPACITY;
    stack->size = 0;

    // Alokacia pamate pre pole prvkov
    stack->elements = malloc(stack->capacity * sizeof(StackElem));

    // Kontrola, ci sa alokacia podarila
    if (stack->elements == NULL)
    {
        // Chyba
        return;
    }
}

// Funkcia na kontrolu a zmenu velkosti zasobnika, ak je to potrebne
void Stack_CheckSize(Stack *stack)
{
    // Vypocet pomeru naplnenia zasobnika
    double fillRatio = (double)stack->size / stack->capacity;

    // Ak pomer naplnenia presahuje realokacny faktor
    if (fillRatio >= STACK_REALLOC_FACTOR)
    {
        // Zdvojnasob kapacitu zasobnika
        size_t newSize = stack->capacity * 2;

        // Realokacia pamate pre pole prvkov
        StackElem *newArray = realloc(stack->elements, newSize * sizeof(StackElem));

        // Kontrola, ci sa realokacia podarila
        if (newArray == NULL)
        {
            // Chyba
            return;
        }

        // Aktualizacia vlastnosti zasobnika
        stack->elements = newArray;
        stack->capacity = newSize;
    }
    // Ak pomer naplnenia je nizsi ako deallokacny faktor a kapacita je vacsia ako pociatocna kapacita
    else if (fillRatio <= STACK_DEALLOC_FACTOR && stack->capacity > STACK_INITIAL_CAPACITY)
    {
        // Zredukujte kapacitu zasobnika na polovicu
        size_t newSize = stack->capacity / 2;

        // Realokacia pamate pre pole prvkov
        StackElem *newArray = realloc(stack->elements, newSize * sizeof(StackElem));

        // Kontrola, ci sa realokacia podarila
        if (newArray == NULL)
        {
            // Chyba
            return;
        }

        // Aktualizacia vlastnosti zasobnika
        stack->elements = newArray;
        stack->capacity = newSize;
    }
}

// Funkcia na kontrolu, ci je zasobnik prazdny
bool Stack_IsEmpty(const Stack *stack)
{
    // Vrati true, ak je velkost zasobnika 0, inak false
    return stack->size == 0;
}

// Funkcia na ziskanie prvku na vrchole zasobnika
void Stack_Top(const Stack *stack, StackElem *dataPtr)
{
    // Ak zasobnik nie je prazdny
    if (!Stack_IsEmpty(stack))
    {
        // Priradi hodnotu prvku na vrchole zasobnika do dataPtr
        *dataPtr = stack->elements[stack->topIndex];
    }
}

// Funkcia na odstranenie prvku z vrcholu zasobnika
void Stack_Pop(Stack *stack)
{
    // Ak zasobnik nie je prazdny
    if (!Stack_IsEmpty(stack))
    {
        // Dekrementuj index vrcholu a pocet prvkov v zasobniku
        stack->topIndex--;
        stack->size--;

        // Kontrola velkosti zasobnika
        Stack_CheckSize(stack);
    }
}

// Funkcia na vlozenie prvku na vrchol zasobnika
void Stack_Push(Stack *stack, StackElem element)
{
    // Inkrementuj index vrcholu a pocet prvkov v zasobniku
    stack->topIndex++;
    stack->size++;

    // Pridajte prvok na vrchol zasobnika
    stack->elements[stack->topIndex] = element;

    // Kontrola velkosti zasobnika
    Stack_CheckSize(stack);
}

// Funkcia na uvolnenie struktury
void Stack_Dispose(Stack *stack)
{
    // Uvolenenie pamate alokovanej pre pole prvkov
    free(stack->elements);

    stack->topIndex = -1;
    stack->elements = NULL;
}

// Funkcia na vypis stavu a prvkov zasobnika
void Stack_Print(const Stack *stack)
{
    // Maximalny index zasobnika
    size_t maxIndex = (size_t)stack->topIndex;

    // Vypis stavu zasobnika
    printf("[STACK] Status:\n");
    printf("Size: %zu | Capacity: %zu | TopIndex: %d\n", stack->size, stack->capacity, stack->topIndex);

    // Ak nie je zasobnik inicializovany
    if (stack->elements == NULL)
    {
        printf("[STACK Error]: NOT INITIALIZED\n");
        return;
    }

    // Vypis prvkov zasobnika od spodu po vrch
    printf("STACK: BOTTOM [");
    if (!Stack_IsEmpty(stack))
    {
        for (size_t i = 0; i < maxIndex; i++)
        {
            printf("<%s, %s, %d, %d>, ", stack->elements[i].string, stack->elements[i].relation, stack->elements[i].id, stack->elements[i].value);
        }
    }
    printf(" ] TOP\n");
}