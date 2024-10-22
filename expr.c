/**
 * IFJ-IAL Projekt 2023
 *
 * @file expr.c
 * @brief Precedencna analyza vyrazov
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
    E, // ==
    U, // Undefined
};

int precedenceTable[PRETABLESIZE][PRETABLESIZE] = {
    /*! *  /  +  -  == != <  > <=  >= ?? (  )  i  $ */
    {U, R, R, R, R, R, R, R, R, R, R, R, U, R, U, R}, // !
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
    {L, L, L, L, L, L, L, L, L, L, L, L, L, R, L, R}, // ??
    {L, L, L, L, L, L, L, L, L, L, L, L, L, E, L, U}, // (
    {R, R, R, R, R, R, R, R, R, R, R, R, U, R, U, R}, // )
    {R, R, R, R, R, R, R, R, R, R, R, R, U, R, U, R}, // i
    {L, L, L, L, L, L, L, L, L, L, L, L, L, L, L, E}, // $ (EOF, Stack bottom)
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
        return -1; // Ostatne (syntax error)
    }
}

bool tokenIsTerm(token_t token)
{
    switch (token.type)
    {
    case TOK_IDENTIFIER:
    case TOK_INT:
    case TOK_DOUBLE:
    case TOK_STRING:
    case TOK_KW_NIL:
        return true;
    default:
        return false;
    }
}

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

bool tokenIsRelationalOperator(token_t token)
{
    switch (token.type)
    {
    case TOK_EQUAL:
    case TOK_NOT_EQUAL:
    case TOK_LESSER:
    case TOK_GREATER:
    case TOK_LESSER_OR_EQUAL:
    case TOK_GREATER_OR_EQUAL:
        return true;
    default:
        return false;
    }
}

token_type_t getTokenType(token_t *token, local_symtab_w_par_ptr_t *table, global_symtab_t *globalTable)
{
    local_symtab_t *search;
    search = local_search_in_all(table, &token->attribute.str);
    if (search == NULL)
    {
        global_symtab_t *globalSearch;
        globalSearch = global_search(globalTable, &token->attribute.str);
        if (globalSearch == NULL)
        {
            // ERROR 5, neda sa odvodit typ z tabulky symbolov
            fprintf(stderr, "[EXPR] ERROR: Unknown identifier type\n");
            returnError(VARIABLE_DEFINITION_ERR);
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
            // ERROR 5, neda sa odvodit typ z tabulky symbolov
            fprintf(stderr, "[EXPR] ERROR: SYMTABLE Unknown identifier type\n");
            returnError(VARIABLE_DEFINITION_ERR);
        }
    }
    // Zachovanie atributu includesNil
    token->attribute.includesNil = search->includesNil;
    return search->type;
}

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
        returnError(TYPE_COMPATIBILITY_ERR); // Z typu nil sa neda odvodit typ
    }
    // Operandy ok
    return true;
}

bool dataTypeEqual(token_t operand1, token_t operand2, token_t operation)
{
    // Extra kontroly pre operator '??'
    if (operation.type == TOK_DOUBLE_QUEST_MARK)
    {
        // Obidva operandy obsahuju nil -> error
        if (operand1.tree->token.attribute.includesNil == true && operand2.tree->token.attribute.includesNil == true) // AST strom nebude obsahovat nil, zlyhal by uz skor
        {
            fprintf(stderr, "[EXPR] ERROR: `??` operator, where both of the operands include nil\n");
            returnError(TYPE_COMPATIBILITY_ERR);
        }
        // Oba operatory su TOK_KW_NIL
        else if (operand1.type == TOK_KW_NIL && operand2.type == TOK_KW_NIL)
        {
            fprintf(stderr, "[EXPR] ERROR: ?? operator : type compatibility error, type cannot be deducted from nil - 4 ?? nil\n");
            returnError(TYPE_COMPATIBILITY_ERR);
        }
        // Lavy operand obsahuje nil a pravy nie alebo lavy je TOK_KW_NIL a pravy nie
        else if (operand1.attribute.includesNil == true && operand2.attribute.includesNil == false)
        {
            // ok, skontroluj typy
        }
        else
        {
            fprintf(stderr, "[EXPR] ERROR: ?? operator\n");
            returnError(TYPE_COMPATIBILITY_ERR);
        }

        // Kontrola typov, typy sa nerovnaju a operand2 nie je literal, takze sa neda pretypovat
        // Hlbsia kontrola prebieha pri vytvarani stromu
        if ((operand1.tree->type != operand2.tree->type) && operand2.tree->literal == false)
        {
            fprintf(stderr, "[EXPR] ERROR: ?? operator : type compatibility error, types are not equal!\n");
            returnError(TYPE_COMPATIBILITY_ERR);
        }
    }

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
        fprintf(stderr, "[EXPR] ERROR: Incompatible data types - Int + Double, where none of them is literal\n");
        returnError(TYPE_COMPATIBILITY_ERR);
    }

    if (operation.type == TOK_EQUAL || operation.type == TOK_NOT_EQUAL)
    {
        if ((operand1.tree->type == TOK_STRING && operand2.tree->token.attribute.includesNil == true) ||
            (operand2.tree->type == TOK_STRING && operand1.tree->token.attribute.includesNil == true))
        {
            return true;
        }
        else if (operand1.tree->type == TOK_STRING && operand2.tree->type == TOK_STRING)
        {
            return true;
        }
        else if (operand1.tree->type == TOK_STRING || operand2.tree->type == TOK_STRING)
        {
            fprintf(stderr, "[EXPR] ERROR: Incompatible data types - String == notString\n");
            returnError(TYPE_COMPATIBILITY_ERR);
        }
    }

    if (operand1.tree->type == TOK_STRING && operand2.tree->type == TOK_STRING)
    {
        if (tokenIsRelationalOperator(operation))
        {
            return true;
        }
        else if (operation.type == TOK_MINUS || operation.type == TOK_DIV || operation.type == TOK_MUL)
        {
            fprintf(stderr, "[EXPR] ERROR: Invalid operator, String -/* String\n");
            returnError(TYPE_COMPATIBILITY_ERR);
        }
    }
    else if (operand1.tree->type == TOK_STRING || operand2.tree->type == TOK_STRING)
    {
        fprintf(stderr, "[EXPR] ERROR: Incompatible data types - String + notString\n");
        returnError(TYPE_COMPATIBILITY_ERR); // TODO : Kontrola napr. 5 + "string"
    }
    return true;
}

void reduceArithmetic(Stack *stack)
{
    token_t stackTop;
    Stack_Top(stack, &stackTop);
    token_t operand1 = stack->elements[stack->size - 3];
    token_t operation = stack->elements[stack->size - 2];
    token_t operand2 = stack->elements[stack->size - 1];

    if (checkOperands(operand1, operand2))
    {
        dataTypeEqual(operand1, operand2, operation);
    }

    Stack_PopUntilLesser(stack);

    token_t expr = operation;
    // Vytvorenie stromu s uchovanim datoveho typu po operacii
    expr.tree = make_tree(expr, operand1.tree, operand2.tree, false);
    expr.terminal = false;

    Stack_Push(stack, &expr);
}

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
            ((operand1.type == TOK_KW_NIL || operand1.attribute.includesNil == true) &&
             (operand2.type == TOK_KW_NIL || operand2.attribute.includesNil == true)))
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
        else if ((operation.type == TOK_EQUAL || operation.type == TOK_NOT_EQUAL) &&
                 (operand1.attribute.includesNil == true || operand2.attribute.includesNil == true) &&
                 dataTypeEqual(operand1, operand2, operation))
        {
            // Jeden z operandov moze zahrnovat nil, typy sa nemusia rovnat, ak sa daju pretypovat
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

    if (operand1.tree->literal == true)
    {
        fprintf(stderr, "[EXPR] ERROR: Force unwrap literal\n");
        returnError(TYPE_COMPATIBILITY_ERR);
    }

    token_t expr = operation;
    expr.terminal = false;
    expr.tree = make_tree(operation, operand1.tree, NULL, false);
    expr.tree->token.attribute.includesNil = false;
    Stack_Push(stack, &expr);
}

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

void reduceTerm(Stack *stack)
{
    token_t operand;
    Stack_Top(stack, &operand);
    Stack_PopUntilLesser(stack);
    operand.terminal = false;
    Stack_Push(stack, &operand);
    return;
}

void applyRule(Stack *stack)
{
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
    else if (tokenIsTerm(stackTop))
    {
        // E → i
        reduceTerm(stack);
        return;
    }
    else
    {
        fprintf(stderr, "[EXPR] ERROR: Unknown rule\n");
        returnError(SYNTAX_ERR);
        return;
    }
}

bool expressionEnd(token_t *token, token_t prevToken, bool condition)
{
    // Obycajny koniec riadku, napr. if (a == 5), if podmienka bez zatvoriek napr. if a == 5 {}
    if ((token->type == TOK_EOL && (!tokenIsOperator(prevToken) || prevToken.type == TOK_NOT)) || token->type == TOK_EOF || token->type == TOK_COMMENT || token->type == TOK_BLOCK_COM_START || token->type == TOK_L_CRL_BRCKT || token->type == TOK_R_CRL_BRCKT)
    {
        // Koniec podmienky, napr. if (a == 5) { ... }
        if (condition == true && token->type != TOK_L_CRL_BRCKT)
        {
            while (token->type == TOK_EOL)
            {
                *token = getToken();
                if (token->type == TOK_L_CRL_BRCKT)
                {
                    break;
                }
            }
            ungetToken();
            ungetToken();
            *token = getToken();
        }
        return true;
    }
    while (token->type == TOK_EOL)
        *token = getToken();
    // Napriklad vyraz na viacero riadkov, kde pred EOL je operator
    return false;
}

int getPrecedence(token_t stackTop, token_t token)
{
    int incomingToken = getTokenIndex(token);
    if (incomingToken == -1)
    {
        fprintf(stderr, "[EXPR] ERROR: Unknown token\n");
        returnError(SYNTAX_ERR);
    }

    return precedenceTable[getTokenIndex(stackTop)][incomingToken];
}

ast_node_t *checkExpression(local_symtab_w_par_ptr_t *table, global_symtab_t *globalTable, bool isCondition)
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

    while (expressionEnd(&token, prevToken, isCondition) == false)
    {
        token.terminal = true;

        token_t *stackTop;
        stackTop = Stack_GetTopTerminal(&stack);

        int precedence = getPrecedence(*stackTop, token);

        // LOAD
        if (precedence == L)
        {
            // Vychodzia hodnota
            token.attribute.includesNil = false;

            // Nastavenie atributu includesNil pre nil
            if (token.type == TOK_KW_NIL)
                token.attribute.includesNil = true;

            // Uchovanie informacii o povodnom tokene
            token.terminal = true;
            token.tree = make_leaf(token);

            // Ziskanie typu tokenu z tabulky symbolov
            if (token.type == TOK_IDENTIFIER)
            {
                token.type = getTokenType(&token, table, globalTable);
                // Token bol identifikator
                token.tree->literal = false;
            }
            else
            {
                // Token je literal
                token.tree->literal = true;
            }
            // Ulozenie typu tokenu do stromu, pri operaciach sa prepisuje !!
            token.tree->type = token.type;
            token.tree->token.attribute.includesNil = token.attribute.includesNil;
            Stack_InsertLesser(&stack);
            Stack_Push(&stack, &token);

            prevToken = token;
            token = getToken();
            continue;
        }
        // REDUCE - pouzi pravidlo
        else if (precedence == R)
        {
            applyRule(&stack);
        }
        else if (precedence == E)
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
            fprintf(stderr, "[EXPR] ERROR: Undefined precedence\n");
            Stack_Dispose(&stack);
            returnError(SYNTAX_ERR);
        }
    }

    // Pokusaj sa redukovat vysledok az pokym stack != '$E'
    while ((token.type == TOK_EOF || token.type == TOK_R_BRCKT || token.type == TOK_EOL || token.type == TOK_COMMENT || token.type == TOK_BLOCK_COM_START || token.type == TOK_L_CRL_BRCKT) && stack.size != 2)
    {
        applyRule(&stack);
    }

    // Vysledok je na vrchole zasobnika, resp. koren AST stromu
    token_t result;
    Stack_Top(&stack, &result);
    gen_expr(output, result.tree);
    Stack_Dispose(&stack);

    // Vratenie tokenu spat do parseru, pre dalsiu pripadnu analyzu
    ungetToken();

    // Ak sme v podmienke, vyraz musi obsahovat relacny operator, inak chyba 7
    if (isCondition == true)
    {
        if (result.type != TOK_EQUAL && result.type != TOK_NOT_EQUAL && result.type != TOK_LESSER && result.type != TOK_GREATER && result.type != TOK_LESSER_OR_EQUAL && result.type != TOK_GREATER_OR_EQUAL)
        {
            fprintf(stderr, "[EXPR] ERROR: No comparison operator in condition!\n");
            returnError(TYPE_COMPATIBILITY_ERR);
        }
    }
    return result.tree;
}