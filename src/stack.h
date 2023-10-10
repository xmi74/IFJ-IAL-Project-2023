#include <stdlib.h>
#include <stdbool.h>

#define STACK_INITIAL_CAPACITY 10
#define STACK_REALLOC_FACTOR 0.8
#define STACK_DEALLOC_FACTOR 0.3

typedef struct
{
    char *string;
    char *relation;
    int id;
    int value;
} StackElem;

typedef struct
{
    int topIndex;
    size_t capacity;
    size_t size;
    StackElem *elements;
} Stack;

void Stack_Error(int);

void Stack_Init(Stack *);

bool Stack_IsEmpty(const Stack *);

bool Stack_IsFull(const Stack *);

void Stack_Top(const Stack *, StackElem *);

void Stack_Pop(Stack *);

void Stack_Push(Stack *, StackElem);

void Stack_Dispose(Stack *);