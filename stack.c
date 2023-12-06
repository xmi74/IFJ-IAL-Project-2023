/**
 * IFJ-IAL Projekt 2023
 *
 * @file stack.c
 * @brief Implementacia zasobnika tokenov
 *
 * @author Igor Mikula (xmikul74)
 * @author Marko Olesak (xolesa00)
 * @author Jan Findra (xfindr01)
 * @author Tomas Arlt (xarltt00)
 */

#include "stack.h"

/**
 * @brief Inicializacia zasobnika
 * @param stack Ukazatel na zasobnik
 * @return void
*/
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
    if (stack->elements == NULL)
    {
        returnError(INTERN_ERR);
    }

    // Kontrola, ci sa alokacia podarila
    if (stack->elements == NULL)
    {
        // ERROR
        return;
    }
}

/**
 * @brief Kontrola a zmena velkosti zasobnika, ak je to potrebne
 * @param stack Ukazatel na zasobnik
 * @return void
*/
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
            returnError(INTERN_ERR);
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
            returnError(INTERN_ERR);
        }

        // Aktualizacia vlastnosti zasobnika
        stack->elements = newArray;
        stack->capacity = newSize;
    }
}

/**
 * @brief Funkcia na zistenie, ci je zasobnik prazdny
 * Funkcia vrati true, ak je velkost zasobnika 0, inak false
 * @param stack Ukazatel na zasobnik
 * @return bool
*/
bool Stack_IsEmpty(const Stack *stack)
{
    // Vrati true, ak je velkost zasobnika 0, inak false
    return stack->size == 0;
}

/**
 * @brief Funkcia na zistenie prvku na vrchole zasobnika
 * Funkcia nahra do dataPtr adresu prvku na vrchole zasobnika
 * @param stack Ukazatel na zasobnik
*/
void Stack_Top(const Stack *stack, token_t *dataPtr)
{
    // Ak zasobnik nie je prazdny
    if (!Stack_IsEmpty(stack))
    {
        // Priradi hodnotu prvku na vrchole zasobnika do dataPtr
        *dataPtr = stack->elements[stack->topIndex];
    }
}

/**
 * @brief Funkcia na odstranenie prvku z vrcholu zasobnika
 * @param stack Ukazatel na zasobnik
 * @return void
*/
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

/**
 * @brief Funkcia na vlozenie prvku na vrchol zasobnika
 * @param stack Ukazatel na zasobnik
 * @param element Ukazatel na pridavany prvok
 * @return void
*/
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
/**
 * @brief Funkcia na uvolnenie struktury
 * @param stack Ukazatel na zasobnik
 * @return void
*/
void Stack_Dispose(Stack *stack)
{
    // Uvolenenie pamate alokovanej pre pole prvkov
    free(stack->elements);

    stack->topIndex = -1;
    stack->elements = NULL;
}

/**
 * @brief Funkcia vracia najvyssi token, ktory je terminal
 * @param stack Ukazatel na zasobnik
 * @return token_t
*/
token_t *Stack_GetTopTerminal(Stack *stack)
{
    token_t *currToken = NULL;
    for (int i = stack->size - 1; i >= 0; i--)
    {
        currToken = &stack->elements[i];
        if (currToken->terminal)
        {
            return currToken;
        }
    }
    return NULL;
}

/**
 * @brief Funkcia sluzi na vlozenie '<' pred prvy terminal v zasobniku.
 * Vyhlada index vlozenia, posunie vsetky prvky o jedno miesto doprava a vlozi '<'.
 * @param stack Ukazatel na zasobnik
 * @return void
*/
void Stack_InsertLesser(Stack *stack)
{
    token_t lesser;
    lesser.type = TOK_NOTHING;
    lesser.terminal = false;

    token_t currToken;
    Stack_Top(stack, &currToken);

    // Vyhladanie indexu vlozenia
    int currIndex = -1;
    for (int i = stack->size - 1; i >= 0; i--)
    {
        currToken = stack->elements[i];
        if (currToken.terminal)
        {
            currIndex = i;
            break;
        }
    }

    if (currIndex != -1)
    {
        // Zvacsenie zasobnika a inkrementacia topIndexu pre posunutie prvkov
        stack->size++;
        stack->topIndex++;

        // Kontrola velkosti zasobnika
        Stack_CheckSize(stack);

        // Posunutie prvkov
        for (int i = stack->size - 1; i > currIndex; i--)
        {
            stack->elements[i] = stack->elements[i - 1];
        }

        // Vlozenie '<' na spravne miesto
        stack->elements[currIndex + 1] = lesser;

        return;
    }
}

/**
 * @brief Funkcia sluzi na odstranenie vsetkych prvkov z vrcholu zasobnika az po najblizsi terminal.
 * @param stack Ukazatel na zasobnik
 * @return void
*/
void Stack_PopUntilLesser(Stack *stack)
{
    token_t stackTop;
    Stack_Top(stack, &stackTop);
    while (stackTop.type != TOK_NOTHING || stackTop.terminal == true)
    {
        Stack_Pop(stack);
        Stack_Top(stack, &stackTop);
    }

    Stack_Pop(stack);
}
