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

/**
 * @brief Pomocna funkcia pre zistenie indexu tokenu v precedencnej tabulke.
 *
 * @param token Token, ktoreho index sa ma zistit.
 * @return Index tokenu v precedencnej tabulke.
 */
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

/**
 * @brief Funkcia zistuje ci su dva tokenu rovnakeho datoveho typu.
 * @param token1 Prvy token.
 * @param token2 Druhy token.
 * @return True ak su datove typy rovnake, inak false.
 */
bool dataTypeEqual(token_t token1, token_t token2)
{
    // TODO : 4.5 != 5.0 * 4.4
    if (token1.type == TOK_INT && token2.type == TOK_INT)
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
    return false;
}

// RULES:
// 1: E → !E
// 2: E → E*E
// 3: E → E/E
// 4: E → E+E
// 5: E → E-E
// 6: E → E==E
// 7: E → E!=E
// 8: E → E<E
// 9: E → E>E
// 10: E → E<=E
// 11: E → E>=E
// 12: E → E??E
// 13: E → (E)
// 14: E → i

/**
 * @brief pomocna funckia pre zistenie ci je token identifikator
 * @param token token ktory sa ma skontrolovat
 * @return true ak je identifikator, inak false
 */
bool tokenIsIdentifier(token_t token)
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

/**
 * @brief Funkcia na zistenie typu tokenu z tabulky symbolov
 * @param token token ktoreho typ sa ma zistit
 * @param table tabulka symbolov
 * @return typ tokenu
 */
token_type_t getTokenType(token_t token, local_symtab_t *table)
{
    local_symtab_t *search;
    search = local_search(table, &token.attribute.str);
    if (search != NULL)
    {
        return search->type;
    }
    else
    {
        printf("[EXPR] ERROR: Unknown identifier type\n");
        return TOK_EOF;
    }
}

/**
 * @brief Funkcia na redukciu aritmetickych vyrazov
 *
 * Funkcia tiez tvori uzle stromu
 *
 * @param stack zasobnik
 * @return void
 */
void reduceArithmetic(Stack *stack)
{
    // TODO : Mozno aj tu treba najst datovy typ pre generator?
    token_t stackTop;
    Stack_Top(stack, &stackTop);
    token_t operand2 = stack->elements[stack->size - 1];
    token_t operand1 = stack->elements[stack->size - 3];

    token_t operation = stack->elements[stack->size - 2];

    while (stackTop.type != TOK_LESSER || stackTop.terminal == true)
    {
        Stack_Pop(stack);
        Stack_Top(stack, &stackTop);
    }

    Stack_Pop(stack);

    token_t expr = operation;
    expr.tree = (void *)make_tree(expr, operand1.tree, operand2.tree);
    // printf("Tree operands: %s, %s\n", getTokenTypeName(operand1.type), getTokenTypeName(operand2.type));
    expr.terminal = false;

    Stack_Push(stack, &expr);
}

/**
 * @brief Funkcia na redukciu logickych vyrazov
 * Funckia tiez tvori uzle stromu
 * @param stack zasobnik
 * @param table tabulka symbolov
 * @return true ak sa podarila redukcia, resp. datove typy sa rovnaju, inak false
 */
bool reduceLogical(Stack *stack, local_symtab_t *table)
{
    // TODO : String?, Int?, Double? - neda sa odvodit typ
    token_t stackTop;
    Stack_Top(stack, &stackTop);

    token_t operand2 = stack->elements[stack->size - 1];
    token_t operation = stack->elements[stack->size - 2];
    token_t operand1 = stack->elements[stack->size - 3];

    // printf("[EXPR] Identifier : operand1: %s, operand2: %s\n", getTokenTypeName(operand1.type), getTokenTypeName(operand2.type));
    // Jeden z operandov je identifikator, potrebujeme zistit jeho datovy typ
    if (operand1.type == TOK_IDENTIFIER || operand2.type == TOK_IDENTIFIER)
    {
        if (operand1.type == TOK_IDENTIFIER)
        {
            operand1.type = getTokenType(operand1, table);
            if (operand1.type == TOK_EOF)
            {
                printf("[EXPR] - SYMTABLE ERROR: Unknown identifier type\n");
                return false;
            }
        }
        else
        {
            operand2.type = getTokenType(operand2, table);
            if (operand2.type == TOK_EOF)
            {
                printf("[EXPR] - SYMTABLE ERROR: Unknown identifier type\n");
                exit(TYPE_COMPATIBILITY_ERR); // ERROR 7, neda sa odvodit typ z tabulky symbolov
                return false;
            }
        }
    }

    if (dataTypeEqual(operand1, operand2) == false)
    {
        printf("[EXPR] ERROR: Incompatible data types\n");
        exit(TYPE_COMPATIBILITY_ERR); // ERROR 7, datove typy sa nezhoduju
        return false;
    }

    while (stackTop.type != TOK_LESSER || stackTop.terminal == true)
    {
        Stack_Pop(stack);
        Stack_Top(stack, &stackTop);
    }

    Stack_Pop(stack);

    token_t expr = operation;
    expr.tree = (void *)make_tree(operation, operand1.tree, operand2.tree);
    printf("Tree operands: %s, %s\n", getTokenTypeName(operand1.type), getTokenTypeName(operand2.type));
    expr.terminal = false;

    Stack_Push(stack, &expr);
    return true;
}

void reduceNot(Stack *stack)
{
    token_t stackTop;
    Stack_Top(stack, &stackTop);
    token_t operand1 = stack->elements[stack->size - 2];  // E
    token_t operation = stack->elements[stack->size - 1]; // !
    while (stackTop.type != TOK_LESSER || stackTop.terminal == true)
    {
        Stack_Pop(stack);
        Stack_Top(stack, &stackTop);
    }

    Stack_Pop(stack);

    token_t expr = operation;
    expr.terminal = false;
    expr.tree = (void *)make_tree(operation, operand1.tree, NULL);
    // printf("Tree operands: %s\n", getTokenTypeName(operand1.type));
    Stack_Push(stack, &expr);
}

/**
 * @brief Funkcia na aplikaciu pravidiel
 * @param stack zasobnik
 * @param table tabulka symbolov
 * @return true ak sa aplikacia pravidla podarila, ak pravidlo neexistuje alebo nastala chyba pri redukcii, tak false
 */
bool applyRule(Stack *stack, local_symtab_t *table)
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
            if (reduceLogical(stack, table) != true)
            {
                printf("[EXPR] ERROR: Incompatible data types\n");
                return false;
            }
            return true;
        }
        else if (operation.type == TOK_NOT)
        {
            reduceNot(stack);
            return true;
        }
        else
        {
            printf("[EXPR] ERROR: Unknown rule -> (operation)\n");
            return false;
        }
    }
    else if (stackTop.type == TOK_NOT)
    {
        reduceNot(stack);
        return true;
    }
    else
    {
        printf("[EXPR] ERROR: Unknown rule\n");
        return false;
    }
}

/**
 * @brief Funkcia na redukciu zatvoriek
 * @param stack zasobnik
 * @return void
 */
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

/**
 * @brief Funkcia na analyzu vyrazov
 * Funkcia tiez vola generator kodu, pre vygenerovanie kodu vyrazu z jeho AST stromu
 * @param table tabulka symbolov
 * @return true ak sa analyza podarila, inak false
 */
bool checkExpression(local_symtab_t *table) // TODO: Vyrovnavaci stack, globalna tabulka symbolov, zlozitejsie vyrazy
{
    Stack stack;
    Stack_Init(&stack);

    // Spodok stacku je $E (EOF)
    token_t stackBottom;
    stackBottom.terminal = true;
    stackBottom.type = TOK_EOF;

    Stack_Push(&stack, &stackBottom);

    token_t token = getNextToken();

    while (token.type != TOK_EOL || token.type != TOK_EOF) // TODO : BUDE MUSIET BYT ZMENENE
    {
        // Stack_Print(&stack);

        token_t *stackTop;
        stackTop = Stack_GetTopTerminal(&stack);

        int result = precedenceTable[getTokenIndex(*stackTop)][getTokenIndex(token)];
        printf("[EXPR] Precedence: %d, token: [%s] | stacktop: [%s]\n", result, getTokenTypeName(token.type), getTokenTypeName(stackTop->type));

        // LOAD
        if (result == L)
        {
            // Kontrola prefixoveho NOT '!'
            if (token.type == TOK_NOT)
            {
                token_t stackTrueTop;
                Stack_Top(&stack, &stackTrueTop);

                // Ak je operand postfixoveho 'NOT' nieco ine ako identifikator, ')' alebo terminal, tak je to chyba (neterminal je expression)
                if (!tokenIsIdentifier(stackTrueTop) && stackTrueTop.type != TOK_R_BRCKT && stackTrueTop.terminal != false) // TODO : Mozno nemoze nastat R_BRCKT
                {
                    printf("Stack top: %s, terminal: %s\n", getTokenTypeName(stackTrueTop.type), stackTrueTop.terminal ? "true" : "false");
                    printf("[EXPR] ERROR: Prefix 'NOT' operand\n");
                    return false;
                }
            }

            // Ak je identifikator aplikuj pravidlo E → i
            if (tokenIsIdentifier(token))
            {
                token.terminal = false;
                token.tree = (void *)make_leaf(token);
            }
            else
            {
                Stack_InsertLesser(&stack);
            }
            Stack_Push(&stack, &token);
            token = getNextToken();

            continue;
        }
        // REDUCE - pouzi pravidlo
        else if (result == R)
        {
            if (applyRule(&stack, table) == false)
            {
                printf("[EXPR] FAIL\n");
                Stack_Dispose(&stack);
                return false; // TODO ERROR?
            }
        }
        else if (result == E)
        {
            // Koniec analyzy vyrazu, prebehol OK
            if (stackTop->type == TOK_EOF)
                return true;

            // Inak redukuj zatvorky
            Stack_Push(&stack, &token);
            token = getNextToken();
            reduceParenthesis(&stack);
        }
        else // Undefined
        {
            printf("[EXPR] ERROR: UNDEFINED PRECEDENCE\n");
            Stack_Dispose(&stack);
            return false; // ERROR?
        }
    }

    // Stack_Print(&stack);
    // Pokusaj sa redukovat vysledok az pokym stack != '$E'
    while (token.type == TOK_EOF && stack.size != 2)
    {
        if (applyRule(&stack, table) == false)
        {
            printf("[EXPR] FAIL\n");
            Stack_Dispose(&stack);
            return false; // TODO
        }
    }

    // Stack_Print(&stack);

    // VOLANIE GENERATORU
    token_t result;
    Stack_Top(&stack, &result);
    // printf("[EXPR] RESULT: %s\n", getTokenTypeName(result.type));
    printf("[EXPR] OK\n");
    return true;
}