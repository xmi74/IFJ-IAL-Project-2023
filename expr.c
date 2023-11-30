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

extern string_t *output;

enum
{
    L, // <
    R, // >
    E, // =
    U, // Undefined
};

int precedenceTable[PRETABLESIZE][PRETABLESIZE] = {
    /*! *  /  +  -  == != <  > <=  >= ?? (  )  i  $ */
    {U, R, R, R, R, R, R, R, R, R, R, R, L, R, L, R}, // !
    {L, R, R, R, R, R, R, R, R, R, R, R, L, R, L, R}, // *
    {L, R, R, R, R, R, R, R, R, R, R, R, L, R, L, R}, // /
    {L, L, L, R, R, R, R, R, R, R, R, R, L, R, L, R}, // +
    {L, L, L, R, R, R, R, R, R, R, R, R, L, R, L, R}, // -
    {L, L, L, L, L, U, U, U, U, U, U, R, L, R, L, R}, // ==
    {L, L, L, L, L, U, U, U, U, U, U, R, L, R, L, R}, // !=
    {L, L, L, L, L, U, U, U, U, U, U, R, L, R, L, R}, // <
    {L, L, L, L, L, U, U, U, U, U, U, R, L, R, L, R}, // >
    {L, L, L, L, L, U, U, U, U, U, U, R, L, R, L, R}, // <=
    {L, L, L, L, L, U, U, U, U, U, U, R, L, R, L, R}, // >=
    {L, L, L, L, L, L, L, L, L, L, L, U, L, R, L, R}, // ??
    {L, L, L, L, L, L, L, L, L, L, L, L, L, E, L, U}, // (
    {U, R, R, R, R, R, R, R, R, R, R, R, U, R, U, R}, // )
    {U, R, R, R, R, R, R, R, R, R, R, R, U, R, U, R}, // i
    {L, L, L, L, L, L, L, L, L, L, L, L, L, L, L, E}, // $ (EOF)
};

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
    case TOK_EQUAL:
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
    case TOK_EOL:
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
    else if (token1.tree->type == token2.tree->type)
    {
        // Kontrola typov podla AST stromu, resp. tokeny nemusia byt terminaly
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
token_type_t getTokenType(token_t token, local_symtab_w_par_ptr_t *table, global_symtab_t *globalTable)
{
    local_symtab_t *search;
    search = local_search_in_all(table, &token.attribute.str);
    if (search == NULL)
    {
        global_symtab_t *globalSearch;
        globalSearch = global_search(globalTable, &token.attribute.str);
        if (globalSearch == NULL)
        {
            fprintf(stderr, "[EXPR] ERROR: Unknown identifier type\n");
            returnError(VARIABLE_DEFINITION_ERR); // ERROR 5, neda sa odvodit typ z tabulky symbolov
        }
        switch (globalSearch->type)
        {
        case T_INT:
            return TOK_INT;
        case T_DOUBLE:
            return TOK_DOUBLE;
        case T_STRING:
            return TOK_STRING;
        default:
            return TOK_NOTHING;
        }
    }
    return search->type;
}

bool checkOperands(token_t operand1, token_t operand2)
{
    if (operand1.tree == NULL || operand2.tree == NULL || (operand1.type == TOK_LESSER && operand1.terminal == false) || (operand2.type == TOK_LESSER && operand2.terminal == false))
    {
        returnError(SYNTAX_ERR); // NAPR. var a = * 5
    }
    return true;
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
    token_t stackTop;
    Stack_Top(stack, &stackTop);
    token_t operand2 = stack->elements[stack->size - 1];
    token_t operand1 = stack->elements[stack->size - 3];

    token_t operation = stack->elements[stack->size - 2];

    // Mozno nie je potreba, druha podmienka by mala stacit
    if (checkOperands(operand1, operand2)) // Syntax analyza -> napr. (var a : Int = * 3)

        if (operand1.tree->literal == false && operand2.tree->literal == false && dataTypeEqual(operand1, operand2) == false)
        {
            returnError(SYNTAX_ERR);
        }
    // INTEGER nie je literal a druhy operand je double -> ERROR
    if (operand1.tree->literal == false || operand2.tree->literal == false)
    {
        if ((operand1.tree->literal == false && operand1.type == TOK_INT && operand2.type == TOK_DOUBLE) || (operand2.tree->literal == false && operand2.type == TOK_INT && operand1.type == TOK_DOUBLE))
        {
            fprintf(stderr, "[EXPR] ERROR: Incompatible data types - Int + Double, where Int is not literal\n");
            returnError(TYPE_COMPATIBILITY_ERR);
        }
    }

    // Kontrola konkatenacie stringov
    if (operand1.type == TOK_STRING && operand2.type == TOK_STRING)
    {
        if (operation.type != TOK_PLUS)
        {
            fprintf(stderr, "[EXPR] ERROR: Invalid operator, concat operator is + => String + String\n");
            returnError(TYPE_COMPATIBILITY_ERR); // TODO : KONTROLA? napr. 5 * "string"
        }
    }
    else if (operand1.type == TOK_STRING || operand2.type == TOK_STRING)
    {
        fprintf(stderr, "[EXPR] ERROR: Incompatible data types - String + notString\n");
        returnError(TYPE_COMPATIBILITY_ERR); // TODO : Kontrola napr. 5 + "string"
    }

    while (stackTop.type != TOK_LESSER || stackTop.terminal == true)
    {
        Stack_Pop(stack);
        Stack_Top(stack, &stackTop);
    }

    Stack_Pop(stack);

    token_t expr = operation;
    // Vytvorenie stromu s uchovanim datoveho typu po operacii
    expr.tree = make_tree(expr, operand1.tree, operand2.tree);
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
bool reduceLogical(Stack *stack)
{
    // TODO : String?, Int?, Double? - neda sa odvodit typ
    token_t stackTop;
    Stack_Top(stack, &stackTop);

    token_t operand2 = stack->elements[stack->size - 1];
    token_t operation = stack->elements[stack->size - 2];
    token_t operand1 = stack->elements[stack->size - 3];

    if (checkOperands(operand1, operand2)) // Syntax analyza -> napr. (var a : Int = != 3)
        if (dataTypeEqual(operand1, operand2) == false)
        {
            fprintf(stderr, "[EXPR] ERROR: Incompatible data types in bool expression!\n");
            returnError(TYPE_COMPATIBILITY_ERR); // ERROR 7, datove typy sa nezhoduju
        }

    while (stackTop.type != TOK_LESSER || stackTop.terminal == true)
    {
        Stack_Pop(stack);
        Stack_Top(stack, &stackTop);
    }

    Stack_Pop(stack);

    token_t expr = operation;
    expr.tree = make_tree(operation, operand1.tree, operand2.tree);
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
    expr.tree = make_tree(operation, operand1.tree, NULL);
    Stack_Push(stack, &expr);
}

/**
 * @brief Funkcia na aplikaciu pravidiel
 * @param stack zasobnik
 * @param table tabulka symbolov
 * @return true ak sa aplikacia pravidla podarila, ak pravidlo neexistuje alebo nastala chyba pri redukcii, tak false
 */
void applyRule(Stack *stack)
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
            return;
        }
        // Logicka redukcia
        else if (operation.type == TOK_EQUAL || operation.type == TOK_NOT_EQUAL || operation.type == TOK_LESSER || operation.type == TOK_GREATER || operation.type == TOK_LESSER_OR_EQUAL || operation.type == TOK_GREATER_OR_EQUAL || operation.type == TOK_DOUBLE_QUEST_MARK)
        {
            // Nastala chyba pri redukcii
            reduceLogical(stack);
            return;
        }
        else if (operation.type == TOK_NOT)
        {
            reduceNot(stack);
            return;
        }
        else
        {
            fprintf(stderr, "[EXPR] ERROR: Unknown rule -> (operation)\n");
            returnError(SYNTAX_ERR);
            return;
        }
    }
    else if (stackTop.type == TOK_NOT)
    {
        reduceNot(stack);
        return;
    }
    else
    {
        fprintf(stderr, "[EXPR] ERROR: Unknown rule\n");
        returnError(SYNTAX_ERR);
        return;
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

    Stack_PopUntilLesser(stack);

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
token_type_t checkExpression(local_symtab_w_par_ptr_t *table, global_symtab_t *globalTable) // TODO: Vyrovnavaci stack, globalna tabulka symbolov, zlozitejsie vyrazy
{
    Stack stack;
    Stack_Init(&stack);

    // Spodok stacku je $E (EOF)
    token_t stackBottom;
    stackBottom.terminal = true;
    stackBottom.type = TOK_EOF;

    Stack_Push(&stack, &stackBottom);

    token_t token = getToken();

    int parenCount = 0;

    while (token.type != TOK_EOL || token.type != TOK_EOF) // TODO : BUDE MUSIET BYT ZMENENE
    {
        Stack_Print(&stack);
        token.terminal = true;

        token_t *stackTop;
        stackTop = Stack_GetTopTerminal(&stack);

        int result = precedenceTable[getTokenIndex(*stackTop)][getTokenIndex(token)];

        // Koniec vyrazu v podmienke, napr. if,while a pod.
        if (parenCount == 0 && token.type == TOK_R_BRCKT)
            break;

        // Kontrola poctu zatvoriek iba pri LOAD! `(` sa moze 2 krat odcitat, ked nastane REDUCE
        if (token.type == TOK_L_BRCKT && result == L)
        {
            parenCount++;
        }
        else if (token.type == TOK_R_BRCKT && result == L)
        {
            parenCount--;
        }

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
                    fprintf(stderr, "[EXPR] ERROR: Prefix '!' operand\n");
                    returnError(SYNTAX_ERR);
                }
            }

            // Ak je identifikator aplikuj pravidlo E → i
            if (tokenIsIdentifier(token))
            {
                token.terminal = false;
                token.tree = make_leaf(token);
                // Ziskanie typu tokenu z tabulky symbolov
                if (token.type == TOK_IDENTIFIER)
                {
                    token.type = getTokenType(token, table, globalTable);
                    if (token.type == TOK_EOF)
                    {
                        fprintf(stderr, "[EXPR] - SYMTABLE ERROR: Unknown identifier type\n");
                        returnError(VARIABLE_DEFINITION_ERR); // ERROR 5, neda sa odvodit typ z tabulky symbolov
                    }
                    token.tree->literal = false;
                }
                else
                {
                    token.tree->literal = true;
                }
                token.tree->type = token.type;
            }
            else
            {
                Stack_InsertLesser(&stack);
            }
            Stack_Push(&stack, &token);
            token = getToken();

            continue;
        }
        // REDUCE - pouzi pravidlo
        else if (result == R)
        {
            applyRule(&stack);
        }
        else if (result == E)
        {
            // Koniec analyzy vyrazu, prebehol OK
            if (stackTop->type == TOK_EOF)
                break;

            // Inak redukuj zatvorky
            Stack_Push(&stack, &token);
            token = getToken();
            reduceParenthesis(&stack);
        }
        else // Undefined
        {
            fprintf(stderr, "[EXPR] ERROR: Undefined precedence, probably KW (String, Int, Double, nil...)\n");
            Stack_Dispose(&stack);
            returnError(SYNTAX_ERR);
        }
    }

    Stack_Print(&stack);
    // Pokusaj sa redukovat vysledok az pokym stack != '$E'
    while ((token.type == TOK_EOF || token.type == TOK_R_BRCKT || token.type == TOK_EOL) && stack.size != 2)
    {
        applyRule(&stack);
    }

    token_t result;
    Stack_Top(&stack, &result);
    gen_expr(output, result.tree);
    // ast_gen(result.tree);
    Stack_Dispose(&stack);
    ungetToken();
    return result.tree->type;
}