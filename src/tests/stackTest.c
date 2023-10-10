#include "../stack.c"
#include <assert.h>

int main()
{
    Stack stack;
    StackElem elem;

    Stack_Init(&stack);

    elem.string = "Hello";
    elem.relation = "child";
    elem.id = 1;
    elem.value = 10;

    Stack_Push(&stack, elem);

    elem.string = "World";
    elem.relation = "parent";
    elem.id = 2;
    elem.value = 20;

    Stack_Push(&stack, elem);

    Stack_Print(&stack);

    Stack_Top(&stack, &elem);
    printf("Top Element: <%s, %s, %d, %d>\n", elem.string, elem.relation, elem.id, elem.value);

    Stack_Dispose(&stack);

    // Kontrola inicializacie
    Stack_Init(&stack);
    assert(stack.topIndex == -1);
    assert(stack.capacity == STACK_INITIAL_CAPACITY);
    assert(stack.size == 0);
    assert(stack.elements != NULL);
    Stack_Print(&stack);
    Stack_Dispose(&stack);

    // Kontrola checksize funckie
    Stack_Init(&stack);

    for (int i = 0; i < STACK_INITIAL_CAPACITY; i++)
    {
        StackElem elem;
        elem.string = "Test";
        elem.relation = "equals";
        elem.id = i + 1;
        elem.value = i + 10;
        Stack_Push(&stack, elem);
    }

    // Kapacita by mala byt dvojita
    assert(stack.capacity == STACK_INITIAL_CAPACITY * 2);

    Stack_Print(&stack);

    Stack_Dispose(&stack);

    // Kontrola dealokaice
    Stack_Init(&stack);

    for (int i = 0; i < STACK_INITIAL_CAPACITY * 2; i++)
    {
        StackElem elem;
        elem.string = "Test";
        elem.relation = "equals";
        elem.id = i + 1;
        elem.value = i + 10;
        Stack_Push(&stack, elem);
    }

    for (int i = 0; i < stack.capacity * STACK_DEALLOC_FACTOR - 2; i++)
    {
        Stack_Pop(&stack);
    }

    assert(stack.capacity == STACK_INITIAL_CAPACITY * 2);

    Stack_Print(&stack);

    Stack_Dispose(&stack);

    return 0;
}