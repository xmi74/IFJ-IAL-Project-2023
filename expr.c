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
    case TOK_KW_NIL:
        return 14; // 'i'
    case TOK_EOF:
    case TOK_EOL:
        return 15; // '$'
    default:
        return 16; // Others
    }
}

// RULES:
// 1: E → E!
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
 * @brief pomocna funckia pre zistenie ci je token operator
 * @param token token ktory sa ma skontrolovat
 * @return true ak je operator, inak false
 */
bool tokenIsOperator(token_t token)
{
    switch (token.type)
    {
    case TOK_NOT:
    case TOK_MUL:
    case TOK_DIV:
    case TOK_PLUS:
    case TOK_MINUS:
    case TOK_EQUAL:
    case TOK_NOT_EQUAL:
    case TOK_LESSER:
    case TOK_GREATER:
    case TOK_LESSER_OR_EQUAL:
    case TOK_GREATER_OR_EQUAL:
    case TOK_DOUBLE_QUEST_MARK:
        return true;
    default:
        return false;
    }
}

/**
 * @brief Funkcia na zistenie typu tokenu z tabulky symbolov
 * Funcia tiez nastavuje atribut includesNil
 * @param token token ktoreho typ sa ma zistit
 * @param table tabulka symbolov
 * @return typ tokenu
 */
token_type_t getTokenType(token_t *token, local_symtab_w_par_ptr_t *table, global_symtab_t *globalTable)
{
    // TODO: PREROBIT PRE INCLUDESNIL MOZNO AJ INE ATRIBUTTY
    local_symtab_t *search;
    search = local_search_in_all(table, &token->attribute.str);
    if (search == NULL)
    {
        global_symtab_t *globalSearch;
        globalSearch = global_search(globalTable, &token->attribute.str);
        if (globalSearch == NULL)
        {
            fprintf(stderr, "[EXPR] ERROR: Unknown identifier type\n");
            returnError(VARIABLE_DEFINITION_ERR); // ERROR 5, neda sa odvodit typ z tabulky symbolov
        }
        // Zachovanie atributu includesNil
        token->attribute.includesNil = globalSearch->includesNil;
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
    // Zachovanie atributu includesNil
    token->attribute.includesNil = search->includesNil;
    return search->type;
}
/**
 * @brief Funkcia na kontrolu operandov
 * Funkcia tiez kontroluje ci operandy neobsahuju nil, v takom pripade sa jedna o error
 * @param operand1 prvy operand
 * @param operand2 druhy operand
 * @return true ak su operandy v poriadku, inak false
 */

bool checkOperands(token_t operand1, token_t operand2)
{
    if (operand1.tree == NULL || operand2.tree == NULL || (operand1.type == TOK_LESSER && operand1.terminal == false) || (operand2.type == TOK_LESSER && operand2.terminal == false))
    {
        fprintf(stderr, "[EXPR] ERROR: Incorrect syntax, eg. var a = * 5\n");
        returnError(SYNTAX_ERR); // NAPR. var a = * 5
    }

    if (operand1.tree->token.attribute.includesNil == true || operand2.tree->token.attribute.includesNil == true)
    {
        fprintf(stderr, "[EXPR] ERROR: Possible nil in an expression, type cannot be deducted from nil!\n");
        returnError(TYPE_DEDUCTION_ERR); // Z typu nil sa neda odvodit typ
    }
    // Operandy ok
    return true;
}

/**
 * @brief Funkcia vyhodnocuje kompatibilitu operandov vo vyraze
 * Funkcia zvlast kontroluje operator '??', ktory vyzaaduje specialnu kontrolu
 * @param operand1 Prvy token.
 * @param operand2 Druhy token.
 * @return True ak su datove typy rovnake, inak false.
 */
bool dataTypeEqual(token_t operand1, token_t operand2, token_t operation)
{
    switch (operation.type)
    {
    case TOK_DOUBLE_QUEST_MARK: // '??' operator
        // Vo Swifte nelze, napr let a = 3 ?? 4
        if (operand1.attribute.includesNil == false && operand2.attribute.includesNil == false) // AST strom nebude obsahovat nil, zlyhal by uz skor
        {
            fprintf(stderr, "[EXPR] ERROR: `??` operator, where none of the operands include nil\n");
            returnError(TYPE_COMPATIBILITY_ERR); // Z typu nil sa neda odvodit typ
        }
        // Swift nepodporuje nil ?? nil
        if (operand1.type == TOK_KW_NIL && operand2.type == TOK_KW_NIL)
        {
            fprintf(stderr, "[EXPR] ERROR: Incompatible data types - nil ?? nil\n");
            returnError(OTHER_ERR);
        }
        // Iba jeden z operandov je nil
        else if (operand1.type == TOK_KW_NIL || operand2.type == TOK_KW_NIL)
        {
            return true;
        }
        // Jeden z operandov obsahuje nil, Int?, Double?...
        else if (operand1.attribute.includesNil == true || operand2.attribute.includesNil == true)
        {
            return true;
        }
        break;
    default:
        // Jeden z operandov nie je literal
        if (operand1.tree->literal == false || operand2.tree->literal == false)
        {
            // Kontrola pretypovania typov, kde int musi byt literal
            if ((operand1.tree->literal == false && operand1.tree->type == TOK_INT && operand2.tree->type == TOK_DOUBLE) ||
                (operand2.tree->literal == false && operand2.tree->type == TOK_INT && operand1.tree->type == TOK_DOUBLE))
            {
                fprintf(stderr, "[EXPR] ERROR: Incompatible data types - Int + Double, where Int is not literal\n");
                returnError(TYPE_COMPATIBILITY_ERR);
            }
        }
        // Ani jeden z operandov nie je literal a ich datove typy sa nerovnaju
        else if (operand1.tree->literal == false && operand2.tree->literal == false && operand1.tree->type != operand2.tree->type)
        {
            returnError(TYPE_COMPATIBILITY_ERR);
        }
        return true;
        break;
    }
    return true; // Aby sa nevypisoval warning
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
    if (checkOperands(operand1, operand2))
    { // Syntax analyza -> napr. (var a : Int = * 3)

        dataTypeEqual(operand1, operand2, operation);
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

    Stack_PopUntilLesser(stack);

    token_t expr = operation;
    // Vytvorenie stromu s uchovanim datoveho typu po operacii
    expr.tree = make_tree(expr, operand1.tree, operand2.tree, false);
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

    bool doubleQuestMark = false;
    if (operation.type == TOK_DOUBLE_QUEST_MARK)
    {
        doubleQuestMark = true;
        dataTypeEqual(operand1, operand2, operation);
    }
    else
    {
        // a == nil || a != nil || nil == nil je ok, kde a je Int?, Double?...
        if ((operation.type == TOK_EQUAL || operation.type == TOK_NOT_EQUAL) &&
            ((operand1.type == TOK_KW_NIL && operand2.attribute.includesNil == true) ||
             (operand2.type == TOK_KW_NIL && operand1.attribute.includesNil == true)))
        {
            if (operand1.type == TOK_KW_NIL && operand2.type == TOK_KW_NIL)
            {
                // nil == nil
            }
            else if (operand1.type == TOK_KW_NIL && operand2.attribute.includesNil == true)
            {
                // nil == a, kde a je Int?, Double?...
            }
            else if (operand2.type == TOK_KW_NIL && operand1.attribute.includesNil == true)
            {
                // a == nil, kde a je Int?, Double?...
            }
            else if (operand1.attribute.includesNil == true && operand2.attribute.includesNil == true)
            {
                // a == b, kde a a b su Int?, Double?...
            }
            else
            {
                fprintf(stderr, "[EXPR] ERROR: Incompatible data types\n");
                returnError(TYPE_COMPATIBILITY_ERR);
            }
        }
        else
        {
            // Syntax a sematicka analyza
            if (checkOperands(operand1, operand2))
            {
                dataTypeEqual(operand1, operand2, operation);
            }
        }
    }

    Stack_PopUntilLesser(stack);

    token_t expr = operation;
    expr.tree = make_tree(operation, operand1.tree, operand2.tree, doubleQuestMark);
    expr.terminal = false;

    Stack_Push(stack, &expr);
    return true;
}

/**
 * @brief Funkcia na redukciu NOT operatora
 * @param stack zasobnik
 * @return void
 */
void reduceNot(Stack *stack)
{
    token_t stackTop;
    Stack_Top(stack, &stackTop);
    token_t operand1 = stack->elements[stack->size - 2];  // E
    token_t operation = stack->elements[stack->size - 1]; // !

    Stack_PopUntilLesser(stack);

    // Force unwrap nil
    if (operand1.attribute.includesNil == false)
    {
        fprintf(stderr, "[EXPR] ERROR: Force unwrap nil\n");
        returnError(TYPE_COMPATIBILITY_ERR);
    }

    token_t expr = operation;
    expr.terminal = false;
    expr.tree = make_tree(operation, operand1.tree, NULL, false);
    expr.tree->token.attribute.includesNil = false;
    Stack_Push(stack, &expr);
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
 * @brief Funkcia na aplikaciu pravidiel
 * @param stack zasobnik
 * @param table tabulka symbolov
 * @return true ak sa aplikacia pravidla podarila, ak pravidlo neexistuje alebo nastala chyba pri redukcii, tak false
 */
void applyRule(Stack *stack)
{
    // Stack_Print(stack); // DEBUG
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
 * @brief Funkcia na zistenie ci je koniec vyrazu
 * @param token aktualny token
 * @param prevToken predchadzajuci token
 * @return true ak je koniec vyrazu, inak false
 */
bool expressionEnd(token_t *token, token_t prevToken, bool *condition)
{
    // Obycajny koniec riadku, napr. if (a == 5), if podmienka bez zatvoriek napr. if a == 5 {}
    if ((token->type == TOK_EOL && !tokenIsOperator(prevToken)) || token->type == TOK_EOF || token->type == TOK_L_CRL_BRCKT)
    {
        // Koniec podmienky, napr. if (a == 5) { ... }
        if (token->type == TOK_L_CRL_BRCKT) {
            *condition = true;
        }
        return true;
    }
    if (token->type == TOK_EOL)
        *token = getToken();
    // Napriklad vyraz na viacero riadkov, kde pred EOL je operator
    return false;
}

/**
 * @brief hlavna funkcia na analyzu vyrazov
 * Funkcia tiez vola generator kodu, pre vygenerovanie kodu vyrazu z jeho AST stromu.
 * Funkcia urcuje operaciu podla precedencnej tabulky, resp. ci sa ma vykonat LOAD, REDUCE a pod.
 * @param table tabulka symbolov
 * @param globalTable globalna tabulka symbolov
 * @return ast_node_t* ukazatel na koren AST stromu, kde sa nachadzaju potrebne informacie o vyraze
 */
token_type_t checkExpression(local_symtab_w_par_ptr_t *table, global_symtab_t *globalTable)
{
    Stack stack;
    Stack_Init(&stack);

    // Spodok stacku je $E (EOF)
    token_t stackBottom;
    stackBottom.terminal = true;
    stackBottom.type = TOK_EOF;
    Stack_Push(&stack, &stackBottom);

    token_t token = getToken();
    token_t prevToken = token;

    // Pomocne premenne pre zistenie ci je vyraz v podmienke
    int parenCount = 0;
    bool condition = false;
    while (expressionEnd(&token, prevToken, &condition) == false)
    {
        Stack_Print(&stack); // DEBUG
        token.terminal = true;

        token_t *stackTop;
        stackTop = Stack_GetTopTerminal(&stack);

        token_t stackTrueTop;
        Stack_Top(&stack, &stackTrueTop);
        if (tokenIsIdentifier(stackTrueTop) && tokenIsIdentifier(token))
        {
            fprintf(stderr, "[EXPR] ERROR: Two identifiers in a row\n");
            returnError(SYNTAX_ERR);
        }

        int result = precedenceTable[getTokenIndex(*stackTop)][getTokenIndex(token)];

        // Koniec vyrazu v podmienke, napr. if,while a pod.
        // podpora pre if (a == 5) { ... }
        // a pre if a == 5 { ... }, teda bez zatvoriek
        // if ((parenCount == 0 && token.type == TOK_R_BRCKT) || token.type == TOK_L_CRL_BRCKT)
        // {
        //     condition = true;
        //     break;
        // }

        // // Kontrola poctu zatvoriek iba pri precedencii LOAD! Napr. '(' sa moze 2 krat odcitat, ked nastane REDUCE
        // if (token.type == TOK_L_BRCKT && result == L)
        // {
        //     parenCount++;
        // }
        // else if (token.type == TOK_R_BRCKT && result == L)
        // {
        //     parenCount--;
        // }

        // LOAD
        if (result == L)
        {
            // Kontrola prefixoveho NOT '!' - Syntax analyza
            if (token.type == TOK_NOT)
            {
                token_t stackTrueTop;
                Stack_Top(&stack, &stackTrueTop);

                // Kontrola ci operand je TERM alebo neterminal (Redukovana expression)
                if (tokenIsIdentifier(stackTrueTop) || stackTrueTop.terminal == false)
                {
                    // Kontrola ci je operand literal
                    if (stackTrueTop.tree->literal == true)
                    {
                        fprintf(stderr, "[EXPR] ERROR: Suffix '!' operand cannot be used with a literal\n");
                        returnError(OTHER_ERR);
                    }
                }
                // Oprand je operator (+-*/...)
                else if (stackTrueTop.terminal == true)
                {
                    fprintf(stderr, "[EXPR] ERROR: Prefix '!' operand\n");
                    returnError(SYNTAX_ERR);
                }
            }

            token.attribute.includesNil = false;

            // Ak je identifikator aplikuj pravidlo E → i
            if (tokenIsIdentifier(token) || token.type == TOK_KW_NIL)
            {
                if (token.type == TOK_KW_NIL)
                    token.attribute.includesNil = true;
                // Uchovanie informacii o povodnom tokene
                token.terminal = false;
                token.tree = make_leaf(token);

                // Ziskanie typu tokenu z tabulky symbolov
                if (token.type == TOK_IDENTIFIER)
                {
                    token.type = getTokenType(&token, table, globalTable);
                    if (token.type == TOK_NOTHING) // nemalo by nastat, errory sa volaju vo funkcii
                    {
                        fprintf(stderr, "[EXPR] - SYMTABLE ERROR: Unknown identifier type\n");
                        returnError(VARIABLE_DEFINITION_ERR); // ERROR 5, neda sa odvodit typ z tabulky symbolov
                    }
                    // Token bol premenna, nastav literal na false
                    token.tree->literal = false;
                }
                else
                {
                    // Token je literal
                    token.tree->literal = true;
                }
                // Ulozenie typu tokenu do stromu
                token.tree->type = token.type;
                token.tree->token.attribute.includesNil = token.attribute.includesNil;
            }
            else
            {
                Stack_InsertLesser(&stack);
            }
            Stack_Push(&stack, &token);

            prevToken = token;
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
            prevToken = token;
            token = getToken();
            reduceParenthesis(&stack);
        }
        else // U - Undefined
        {
            fprintf(stderr, "[EXPR] ERROR: Undefined precedence, probably KW (String, Int, Double, nil...) or double '!'\n");
            Stack_Dispose(&stack);
            returnError(SYNTAX_ERR);
        }
    }

    Stack_Print(&stack); // DEBUG
    // Pokusaj sa redukovat vysledok az pokym stack != '$E'
    while ((token.type == TOK_EOF || token.type == TOK_R_BRCKT || token.type == TOK_EOL || token.type == TOK_L_CRL_BRCKT) && stack.size != 2)
    {
        Stack_Print(&stack); // DEBUG
        applyRule(&stack);
    }

    // Vysledok je na vrchole zasobnika, resp. koren AST stromu
    Stack_Print(&stack); // DEBUG
    token_t result;
    Stack_Top(&stack, &result);
    gen_expr(output, result.tree);
    ast_gen(result.tree); // DEBUG
    Stack_Dispose(&stack);

    // Vratenie tokenu spat do parseru, pre dalsiu pripadnu analyzu
    ungetToken();

    // Ak sme v podmienke, vyraz musi obsahovat boolovsky operator, inak chyba 7
    if (condition == true)
    {
        if (result.type != TOK_EQUAL && result.type != TOK_NOT_EQUAL && result.type != TOK_LESSER && result.type != TOK_GREATER && result.type != TOK_LESSER_OR_EQUAL && result.type != TOK_GREATER_OR_EQUAL)
        {
            fprintf(stderr, "[EXPR] ERROR: No bool operator in condition\n");
            returnError(TYPE_COMPATIBILITY_ERR);
        }
    }
    return result.tree->type;
}