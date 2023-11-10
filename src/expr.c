#include "stack.c"

#define PRECEDENCETSIZE 16

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
    {L, R, R, R, R, R, R, R, R, R, R, L, R, R, R, L}, // *
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
    {R, R, R, R, R, R, R, R, R, R, R, R, L, R, U, L}, // i
    {L, L, L, L, L, L, L, L, L, L, L, L, L, U, L, E}, // $
};

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
    case TOK_DOLLAR:
        return 15; // '$'
    default:
        return 16; // Others
    }
}

// RULES:
// 1: E → E+E
// 2: E → E*E
// 3: E → (E)
// 4: E → i

bool checkExpression()
{
    Stack stack;
    Stack_Init(&stack);

    bool error = true;
    // Spodok stacku je $
    token_t *stackBottom;
    stackBottom->type = TOK_DOLLAR;
    dstringInit(&stackBottom->attribute.str);
    dstringAppend(&stackBottom->attribute.str, '$');
    Stack_Push(&stack, stackBottom);

    token_t token = getNextToken();

    while (token.type != TOK_DOLLAR)
    {
        token_t stackTop;
        Stack_Top(&stack, &stackTop);
        Stack_Pop(&stack);
        int result = precedenceTable[getTokenIndex(stackTop)][getTokenIndex(token)];

        if (result == L)
        {
            Stack_Push(&stack, &token);
            token = getNextToken();
        }
        // Redukcia podla pravidiel
        else if (result == R)
        {
            token_t stackTop;
            Stack_Top(&stack, &stackTop);
            while (precedenceTable[getTokenIndex(stackTop)][getTokenIndex(token)] != L)
            {
                Stack_Pop(&stack);
                Stack_Top(&stack, &stackTop);
            }
            if (precedenceTable[getTokenIndex(stackTop)][getTokenIndex(token)] == E)
            {
                // Errorovy stav
                return true;
            }
        }
        else if (result == E)
        {
            // Errorovy stav
            return true;
        }
    }
}
