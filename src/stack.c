/**
 * IFJ-IAL Projekt 2023
 *
 * @file scanner.c
 * @brief Implementacia programu pre lexikalnu analyzu
 *
 * @author Igor Mikula (xmikul74)
 * @author Marko Olesak (xolesa00)
 * @author Jan Findra (xfindr01)
 * @author Tomas Artl (xarltt00)
 */

// TODO : komentare, error handling, viac testov ?

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "stack.h"

void Stack_Init(Stack *stack)
{
    if (stack == NULL)
    {
        // err
        return;
    }

    stack->topIndex = -1;
    stack->capacity = STACK_INITIAL_CAPACITY;
    stack->size = 0;
    stack->elements = malloc(stack->capacity * sizeof(StackElem));

    if (stack->elements == NULL)
    {
        // err
        return;
    }
}

void Stack_CheckSize(Stack *stack)
{
    double fillRatio = (double)stack->size / stack->capacity;

    if (fillRatio >= STACK_REALLOC_FACTOR)
    {
        size_t newSize = stack->capacity * 2;
        StackElem *newArray = realloc(stack->elements, newSize * sizeof(StackElem));

        if (newArray == NULL)
        {
            // err
            return;
        }
        stack->elements = newArray;
        stack->capacity = newSize;
    }
    else if (fillRatio <= STACK_DEALLOC_FACTOR && stack->capacity > STACK_INITIAL_CAPACITY)
    {
        size_t newSize = stack->capacity / 2;
        StackElem *newArray = realloc(stack->elements, newSize * sizeof(StackElem));

        if (newArray == NULL)
        {
            // err
            return;
        }
        stack->elements = newArray;
        stack->capacity = newSize;
    }
}

bool Stack_IsEmpty(const Stack *stack)
{
    return stack->size == 0;
}

void Stack_Top(const Stack *stack, StackElem *dataPtr)
{
    if (!Stack_IsEmpty(stack))
    {
        *dataPtr = stack->elements[stack->topIndex];
    }
}

void Stack_Pop(Stack *stack)
{
    if (!Stack_IsEmpty(stack))
    {
        stack->topIndex--;
        stack->size--;
        Stack_CheckSize(stack);
    }
}

void Stack_Push(Stack *stack, StackElem element)
{
    stack->elements[++stack->topIndex] = element;
    stack->size++;
    Stack_CheckSize(stack);
}

void Stack_Dispose(Stack *stack)
{
    free(stack->elements);
    stack->topIndex = -1;
    stack->elements = NULL;
}

void Stack_Print(const Stack *stack)
{

    size_t maxIndex = (size_t)stack->topIndex;
    printf("[STACK] Status:\n");
    printf("Size: %zu | Capacity: %zu | TopIndex: %d\n", stack->size, stack->capacity, stack->topIndex);

    if (stack->elements == NULL)
    {
        printf("[STACK Error]: NOT INITIALIZED\n");
        return;
    }

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
