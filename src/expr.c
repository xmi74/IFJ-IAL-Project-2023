#include "stack.c"

#define PRECEDENCETSIZE 16

int getTokenIndex(token_t token)
{
    switch (token.type)
    {
    case TOK_NOT:
        return 0; // '!'
    case TOK_MUL:
        return 1; // '*'
    case TOK_DIV:
        return 2; // '/'
    case TOK_PLUS:
        return 3; // '+'
    case TOK_MINUS:
        return 4; // '-'
    case TOK_EQUAL:
        return 5; // '='
    case TOK_NOT_EQUAL:
        return 6; // '!='
    case TOK_LESSER:
        return 7; // '<'
    case TOK_GREATER:
        return 8; // '>'
    case TOK_LESSER_OR_EQUAL:
        return 9; // '<='
    case TOK_GREATER_OR_EQUAL:
        return 10; // '>='
    case TOK_QUESTION:
        return 11; // '?'
    case TOK_L_BRCKT:
        return 12; // '('
    case TOK_R_BRCKT:
        return 13; // ')'
    case TOK_IDENTIFIER:
        return 14; // 'i'
    case TOK_EOF:
        return 15; // '$'
    default:
        return 16; // Others
    }
}

bool dataTypeEqual(token_t token1, token_t token2)
{
    if (token1.type == TOK_IDENTIFIER && token2.type == TOK_IDENTIFIER) // toto neviem
    {
        return true;
    }
    else if (token1.type == TOK_INT && token2.type == TOK_INT)
    {
        return true;
    }
    else if (token1.type == TOK_DOUBLE && token2.type == TOK_DOUBLE)
    {
        return true;
    }
    else if (token1.type == TOK_STRING && token2.type == TOK_STRING)
    {
        return true;
    }
    else
    {
        return false;
    }
}

enum
{
    L, // <
    R, // >
    E, // =
    U, // Undefined
};

int precedenceTable[PRECEDENCETSIZE][PRECEDENCETSIZE] = {
    /*! *  /  +  -  == != <  > <=  >= ?? (  )  i  $ */
    {U, L, L, L, L, L, L, L, L, L, L, R, R, L, R, L}, // !
    {L, R, R, R, R, R, R, R, R, R, R, L, R, R, L, L}, // *
    {L, R, R, R, R, R, R, R, R, R, R, L, R, R, R, L}, // /
    {L, R, R, R, R, R, R, R, R, R, R, L, R, R, R, L}, // +
    {L, R, R, R, R, R, R, R, R, R, R, L, R, R, R, L}, // -
    {L, R, R, R, R, U, U, U, U, U, U, R, R, R, R, L}, // ==
    {L, R, R, R, R, U, U, U, U, U, U, R, R, R, R, L}, // !=
    {L, R, R, R, R, U, U, U, U, U, U, R, R, R, R, L}, // <
    {L, R, R, R, R, U, U, U, U, U, U, R, R, R, R, L}, // >
    {L, R, R, R, R, U, U, U, U, U, U, R, R, R, R, L}, // <=
    {R, L, L, L, L, U, U, U, U, U, U, R, R, R, R, L}, // >=
    {L, L, L, L, L, L, L, L, L, L, L, U, E, R, L, L}, // ??
    {L, L, L, L, L, L, L, L, L, L, L, L, L, E, U, U}, // (
    {R, R, R, R, R, R, R, R, R, R, R, R, R, R, U, L}, // )
    {R, R, R, R, R, R, R, R, R, R, R, R, L, R, U, R}, // i
    {L, L, L, L, L, L, L, L, L, L, L, L, L, U, L, E}, // $ (EOF)
};

// RULES:
// 1: E → E+E
// 2: E → E*E
// 3: E → (E)
// 4: E → i

bool applyRule(Stack *stack)
{
    Stack_Print(stack);
    token_t stackTop;
    Stack_Top(stack, &stackTop);

    // 4. E->i
    if (stackTop.type == TOK_IDENTIFIER)
    {
        token_t prevPopped;
        while (stackTop.type != TOK_LESSER)
        {
            prevPopped = stackTop;
            Stack_Pop(stack);
            Stack_Top(stack, &stackTop);
        }

        Stack_Pop(stack);

        if (prevPopped.type == TOK_IDENTIFIER)
        {
            prevPopped.type = TOK_EXPRESSION;
            Stack_Push(stack, &prevPopped);
        }
        return true;
    }
    else if (stackTop.type == TOK_EXPRESSION || stackTop.type == TOK_R_BRCKT)
    {
        if (stack->elements[stack->size - 2].type == TOK_MUL && stack->elements[stack->size - 3].type == TOK_EXPRESSION)
        {
            // 2. E->E*E
            while (stackTop.type != TOK_LESSER)
            {
                Stack_Pop(stack);
                Stack_Top(stack, &stackTop);
            }

            Stack_Pop(stack);

            token_t expr;
            expr.type = TOK_EXPRESSION;

            Stack_Push(stack, &expr);
            return true;
        }
        else if (stack->elements[stack->size - 2].type == TOK_PLUS && stack->elements[stack->size - 3].type == TOK_EXPRESSION)
        {
            // 1. E->E+E
            while (stackTop.type != TOK_LESSER)
            {
                Stack_Pop(stack);
                Stack_Top(stack, &stackTop);
            }

            Stack_Pop(stack);

            token_t expr;
            expr.type = TOK_EXPRESSION;

            Stack_Push(stack, &expr);
            return true;
        }
        else if (stack->elements[stack->size - 1].type == TOK_L_BRCKT && stack->elements[stack->size - 2].type == TOK_EXPRESSION && stack->elements[stack->size - 3].type == TOK_R_BRCKT)
        {
            // 3. E->(E)
            while (stackTop.type != TOK_LESSER)
            {
                Stack_Pop(stack);
                Stack_Top(stack, &stackTop);
            }

            Stack_Pop(stack);

            token_t expr;
            expr.type = TOK_EXPRESSION;

            Stack_Push(stack, &expr);
            return true;
        } // E -> E/E
        else if (stack->elements[stack->size - 2].type == TOK_DIV && stack->elements[stack->size - 3].type == TOK_EXPRESSION)
        {
            // 2. E->E/E
            while (stackTop.type != TOK_LESSER)
            {
                Stack_Pop(stack);
                Stack_Top(stack, &stackTop);
            }

            Stack_Pop(stack);

            token_t expr;
            expr.type = TOK_EXPRESSION;

            Stack_Push(stack, &expr);
            return true;
        }
        else if (stack->elements[stack->size - 2].type == TOK_MINUS && stack->elements[stack->size - 3].type == TOK_EXPRESSION)
        {
            // 1. E->E-E
            while (stackTop.type != TOK_LESSER)
            {
                Stack_Pop(stack);
                Stack_Top(stack, &stackTop);
            }

            Stack_Pop(stack);

            token_t expr;
            expr.type = TOK_EXPRESSION;

            Stack_Push(stack, &expr);
            return true;
        }
        else if (stack->elements[stack->size - 2].type == TOK_EQUAL && stack->elements[stack->size - 3].type == TOK_EXPRESSION)
        {
            // 1. E->E=E
            while (stackTop.type != TOK_LESSER)
            {
                Stack_Pop(stack);
                Stack_Top(stack, &stackTop);
            }

            Stack_Pop(stack);

            token_t expr;
            expr.type = TOK_EXPRESSION;

            Stack_Push(stack, &expr);
            return true;
        }
        else if (stack->elements[stack->size - 2].type == TOK_NOT_EQUAL && stack->elements[stack->size - 3].type == TOK_EXPRESSION)
        {
            // 1. E->E!=E
            while (stackTop.type != TOK_LESSER)
            {
                Stack_Pop(stack);
                Stack_Top(stack, &stackTop);
            }

            Stack_Pop(stack);

            token_t expr;
            expr.type = TOK_EXPRESSION;

            Stack_Push(stack, &expr);
            return true;
        }
        else
        {
            // Nebolo pouzite ziadne pravidlo
            Stack_Print(stack);
            return false;
        }
    }
    else
    {
        // Nebolo pouzite ziadne pravidlo
        Stack_Print(stack);
        return false;
    }
}

// Kontrolovanie dat typov ? Napr. pri == musia byt rovnake inak chyba...
bool checkExpression()
{
    Stack stack;
    Stack_Init(&stack);

    // Spodok stacku je $E
    token_t stackBottom;
    stackBottom.type = TOK_EOF;

    Stack_Push(&stack, &stackBottom);

    token_t token = getNextToken();

    while (token.type != TOK_EOF)
    {
        Stack_Print(&stack);
        token_t stackTop;
        Stack_Top(&stack, &stackTop);
        int result = precedenceTable[getTokenIndex(stackTop)][getTokenIndex(token)];

        // Load stack
        if (result == L)
        {
            Stack_InsertLesser(&stack);
            Stack_Push(&stack, &token);
            token = getNextToken();
            continue;
        }
        // Redukuj stack
        else if (result == R)
        {
            applyRule(&stack);
            Stack_InsertLesser(&stack);
            Stack_Push(&stack, &token);
            token = getNextToken();
        }
        else if (result == E)
        {
            // Errorovy stav ? alebo ok
            return true;
        }
    }

    // Pokusaj sa redukovat vysledok az pokym stack != '$E'
    while (token.type == TOK_EOF && stack.size != 2)
    {
        if (applyRule(&stack) == false)
        {
            printf("[EXPR] FAIL\n");
            Stack_Dispose(&stack);
            return false;
        }
    }

    Stack_Print(&stack);
    Stack_Dispose(&stack);
    printf("[EXPR] OK\n");
    return true;
}