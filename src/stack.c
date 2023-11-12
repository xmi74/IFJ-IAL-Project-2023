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

#include "stack.h"
#include "debug/printTokenType.c"

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
    stack->elements = malloc(stack->capacity * sizeof(token_t));

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
        token_t *newArray = realloc(stack->elements, newSize * sizeof(token_t));

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
        token_t *newArray = realloc(stack->elements, newSize * sizeof(token_t));

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
void Stack_Top(const Stack *stack, token_t *dataPtr)
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
void Stack_Push(Stack *stack, token_t *element)
{
    // Inkrementuj index vrcholu a pocet prvkov v zasobniku
    stack->topIndex++;
    stack->size++;

    // Pridajte prvok na vrchol zasobnika
    stack->elements[stack->topIndex] = *element;

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

void Stack_Print(Stack *stack)
{
    printf("------ Stack ------\n");
    printf("Size: %ld, capacity : %ld\n", stack->size, stack->capacity);
    for (int i = 0; i < stack->size; i++)
    {
        printf("ID : %d => Type: %s\n", i, getTokenTypeName(stack->elements[i].type));
    }
    printf("-------------------\n");
}

void Stack_InsertLesser(Stack *stack)
{
    token_t lesser;
    lesser.type = TOK_LESSER;

    token_t currToken;
    Stack_Top(stack, &currToken);

    if (currToken.type == TOK_EOF)
    {
        Stack_Push(stack, &lesser);
        return;
    }

    int currIndex = -1;
    for (int i = stack->size - 1; i >= 0; i--)
    {
        currToken = stack->elements[i];
        if (currToken.type != TOK_EXPRESSION && currToken.type != TOK_IDENTIFIER)
        {
            currIndex = i;
            printf("currIndex: %d\n", currIndex);
            Stack_Print(stack);
            break;
        }
    }

    if (currIndex != -1)
    {
        Stack_Print(stack);

        stack->size++;
        stack->topIndex++;
        Stack_CheckSize(stack); // Increase stack size

        for (int i = stack->size - 1; i > currIndex; i--)
        {
            stack->elements[i] = stack->elements[i - 1]; // Shift elements
        }

        stack->elements[currIndex + 1] = lesser; // Add "lesser" token after the current token

        Stack_Print(stack);

        return;
    }
}
