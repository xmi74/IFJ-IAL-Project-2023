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
