/**
 * IFJ-IAL Projekt 2023
 *
 * @file expr.c
 * @brief Bottom-up analyza vyrazov
 *
 * @author Igor Mikula (xmikul74)
 * @author Marko Olesak (xolesa00)
 * @author Jan Findra (xfindr01)
 * @author Tomas Arlt (xarltt00)
 */

#include "expr.h"
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
    case TOK_ASSIGN:
        return 5; // '=='
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
    case TOK_DOUBLE_QUEST_MARK:
        return 11; // '?'
    case TOK_L_BRCKT:
        return 12; // '('
    case TOK_R_BRCKT:
        return 13; // ')'
    case TOK_IDENTIFIER:
    case TOK_STRING:
    case TOK_INT:
    case TOK_DOUBLE:
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

// RULES:
// 1: E → !E
// 2: E → E*E
// 3: E → E/E
// 4: E → E+E
// 5: E → E-E
// 6: E → E==E
// 7: E → E!=E XXX
// 8: E → E<E
// 9: E → E>E
// 10: E → E<=E
// 11: E → E>=E
// 12: E → E??E
// 13: E → (E)
// 14: E → i

bool isIdentifier(token_t token)
{
    switch (token.type)
    {
    case TOK_IDENTIFIER:
    case TOK_INT:
    case TOK_DOUBLE:
    case TOK_STRING:
        return true;
    default:
        return false;
    }
}

// Funckia na redukciu aritmetickych vyrazov
void reduceArithmetic(Stack *stack)
{
    token_t stackTop;
    Stack_Top(stack, &stackTop);
    token_t operand1 = stack->elements[stack->size - 1];
    token_t operand2 = stack->elements[stack->size - 3];

    token_t operation = stack->elements[stack->size - 2];

    while (stackTop.type != TOK_LESSER || stackTop.terminal == true)
    {
        Stack_Pop(stack);
        Stack_Top(stack, &stackTop);
    }

    Stack_Pop(stack);

    token_t expr = operation;
    expr.tree = (void*) make_tree(operation, operand1.tree, operand2.tree);
    expr.terminal = false;

    Stack_Push(stack, &expr);
}

bool reduceLogical(Stack *stack)
{
    // TODO : String?, Int?, Double? - neda sa odvodit typ
    token_t stackTop;
    Stack_Top(stack, &stackTop);
    token_t operand1 = stack->elements[stack->size - 1];
    token_t operand2 = stack->elements[stack->size - 3];

    token_t operation = stack->elements[stack->size - 2];

    if (dataTypeEqual(operand1, operand2) == false)
    {
        printf("[EXPR] ERROR: Incompatible data types\n");
        return false;
    }

    while (stackTop.type != TOK_LESSER || stackTop.terminal == true)
    {
        Stack_Pop(stack);
        Stack_Top(stack, &stackTop);
    }

    Stack_Pop(stack);

    token_t expr = operation;
    expr.tree = (void*) make_tree(operation, operand1.tree, operand2.tree);
    expr.terminal = false;

    Stack_Push(stack, &expr);
    return true;
}

bool applyRule(Stack *stack)
{
    // Stack_Print(stack);
    token_t stackTop;
    Stack_Top(stack, &stackTop);
    token_t operation = stack->elements[stack->size - 2];
    operation.type = stack->elements[stack->size - 2].type;

    if (stackTop.terminal == false)
    {
        // Aritmeticka redukcia
        if (operation.type == TOK_MUL || operation.type == TOK_DIV || operation.type == TOK_PLUS || operation.type == TOK_MINUS)
        {
            reduceArithmetic(stack);
            return true;
        }
        // Logicka redukcia
        else if (operation.type == TOK_ASSIGN || operation.type == TOK_NOT_EQUAL || operation.type == TOK_LESSER || operation.type == TOK_GREATER || operation.type == TOK_LESSER_OR_EQUAL || operation.type == TOK_GREATER_OR_EQUAL)
        {
            // Nastala chyba pri redukcii
            if (reduceLogical(stack) != true)
            {
                printf("[EXPR] ERROR: Incompatible data types\n");
                return false;
            }
            return true;
        }
        else
        {
            printf("[EXPR] ERROR: Unknown operation\n");
            return false;
        }
    }
    else
    {
        printf("[EXPR] ERROR: Unknown rule\n");
        return false;
    }
}

void reduceParenthesis(Stack *stack)
{
    token_t stackTop;
    Stack_Top(stack, &stackTop);
    token_t operand1 = stack->elements[stack->size - 2]; // E
    while (stackTop.type != TOK_LESSER || stackTop.terminal == true)
    {
        Stack_Pop(stack);
        Stack_Top(stack, &stackTop);
    }

    Stack_Pop(stack);

    token_t expr = operand1;
    expr.terminal = false;

    Stack_Push(stack, &expr);
}

// Kontrolovanie dat typov ? Napr. pri == musia byt rovnake inak chyba...
bool checkExpression()
{
    Stack stack;
    Stack_Init(&stack);

    // Spodok stacku je $E (EOF)
    token_t stackBottom;
    stackBottom.terminal = true;
    stackBottom.type = TOK_EOF;

    Stack_Push(&stack, &stackBottom);

    token_t token = getNextToken();

    while (token.type != TOK_EOF) // TODO : BUDE MUSIET BYT ZMENENE
    {
        // Stack_Print(&stack);

        token_t *stackTop;
        stackTop = Stack_GetTopTerminal(&stack);

        int result = precedenceTable[getTokenIndex(*stackTop)][getTokenIndex(token)];
        // printf("[EXPR] Precedence: %d, token: [%s] | stacktop: [%s]\n", result, getTokenTypeName(token.type), getTokenTypeName(stackTop->type));

        // Load stack
        if (result == L)
        {
            if (isIdentifier(token))
            {
                token.terminal = false;
                token.tree = (void *) make_leaf(token);
            }
            else
            {
                Stack_InsertLesser(&stack);
            }
            Stack_Push(&stack, &token);
            token = getNextToken();

            continue;
        }
        // Redukuj stack
        else if (result == R)
        {
            applyRule(&stack);
            // Stack_Push(&stack, &token);
            // token = getNextToken();
        }
        else if (result == E)
        {
            Stack_Push(&stack, &token);
            token = getNextToken();

            reduceParenthesis(&stack);
        }
        else
        {
            // Errorovy stav ? alebo ok
            printf("[EXPR] ERROR: EQUAL\n");
            return true;
        }
    }

    // Stack_Print(&stack);
    //  Pokusaj sa redukovat vysledok az pokym stack != '$E'
    while (token.type == TOK_EOF && stack.size != 2)
    {
        if (applyRule(&stack) == false)
        {
            printf("[EXPR] FAIL\n");
            Stack_Dispose(&stack);
            return false;
        }
    }

    // Stack_Print(&stack);
    Stack_Dispose(&stack);
    printf("[EXPR] OK\n");
    return true;
}