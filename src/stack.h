#include <stdlib.h>
#include <stdbool.h>

// Definicie konstant pre inicializacnu kapacitu zasobnika a faktor prealokacie resp. dealokacie
#define STACK_INITIAL_CAPACITY 10
#define STACK_REALLOC_FACTOR 0.8
#define STACK_DEALLOC_FACTOR 0.3

// Definicia struktury pre prvok zasobnika
typedef struct
{
    char *string;
    char *relation;
    int id;
    int value;
} StackElem;

// Definicia struktury pre samotny zasobnik
typedef struct
{
    int topIndex;        // Index prvku na vrchole zasobnika
    size_t capacity;     // Aktualna kapacita zasobnika
    size_t size;         // Aktualny pocet prvkov v zasobniku
    StackElem *elements; // Pole pre ukladanie prvkov zasobnika
} Stack;

// Prototypy funkcii
void Stack_Error(int);
void Stack_Init(Stack *);
bool Stack_IsEmpty(const Stack *);
bool Stack_IsFull(const Stack *);
void Stack_Top(const Stack *, StackElem *);
void Stack_Pop(Stack *);
void Stack_Push(Stack *, StackElem);
void Stack_Dispose(Stack *);