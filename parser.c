/**
 * IFJ-IAL Projekt 2023
 * 
 * @file parser.c
 * @brief Implementacia programu syntakticke a semanticke analyzy
 * 
 * @author Igor Mikula (xmikul74)
 * @author Marko Olesak (xolesa00)
 * @author Jan Findra (xfindr01)
 * @author Tomas Arlt (xarltt00)
*/

#include "parser.h"

extern token_table_t token_table;
extern string_t *output;

#define MAX_NEST_LEVEL 1000

/**
 * @brief Konvertuje typ z type_t na token_type_t.
 *
 * @param type Typ, ktery se ma konvertovat.
 * @return Konvertovany typ.
 */
token_type_t type_t_to_token_type_t(type_t type)
{
    if (type == T_INT)
    {
        return TOK_INT;
    }
    else if (type == T_DOUBLE)
    {
        return TOK_DOUBLE;
    }
    else if (type == T_STRING)
    {
        return TOK_STRING;
    }
    else
    {
        // nemelo by nastat
        return TOK_NOTHING;
    }
}

/**
 * @brief Konvertuje typ z token_type_t na type_t.
 *
 * @param type Typ, ktery se ma konvertovat.
 * @return Konvertovany typ.
 */
type_t token_type_t_to_type_t(token_type_t type)
{
    if (type == TOK_INT)
    {
        return T_INT;
    }
    else if (type == TOK_DOUBLE)
    {
        return T_DOUBLE;
    }
    else if (type == TOK_STRING)
    {
        return T_STRING;
    }
    else
    {
        // nemelo by nastat
        return T_NIL;
    }
}

/**
 * @brief prevede nazev klicoveho slova na datovy typ.
 *
 * @param kw Klicove slovo.
 * @return Datovy typ.
 */
token_type_t kw_to_token_type(token_type_t kw)
{
    if (kw == TOK_KW_INT)
    {
        return TOK_INT;
    }
    else if (kw == TOK_KW_DOUBLE)
    {
        return TOK_DOUBLE;
    }
    else if (kw == TOK_KW_STRING)
    {
        return TOK_STRING;
    }
    else
    {
        // nemelo by nastat
        return kw;
    }
}

/**
 * @brief Funkce pro preskoceni komentaru.
 *
 */
void ignore_comments()
{
    token_t current_token = getToken();
    if (current_token.type == TOK_BLOCK_COM_START)
    {
        getTokenAssert(TOK_BLOCK_COM_END, SYNTAX_ERR);
    }
    else if (current_token.type == TOK_COMMENT)
    {
        // pass
    }
    else
    {
        ungetToken();
    }
}

/**
 * @brief Nacte vestavene funkce.
 *
 */
void load_built_in_functions()
{
    char funcs[500];
    strcpy(funcs,
    "func write (_ term : String ) {}\n"
    "func readString() -> String? {}\n"
    "func readInt() -> Int? {}\n"
    "func readDouble() -> Double? {}\n"
    "func Int2Double(_ term : Int) -> Double {}\n"
    "func Double2Int(_ term : Double) -> Int {}\n"
    "func substring(of s : String, startingAt i : Int, endingBefore j : Int) -> String? {}\n"
    "func length(_ s : String) -> Int {}\n"
    "func ord(_ c : String) -> Int {}\n"
    "func chr(_ i : Int) -> String {}\n");

    for (int i = strlen(funcs)-1; i >= 0; i--)
    {
        ungetc(funcs[i], stdin);
    }
}

/**
 * @brief Zpracovani volani funkce
 *
 * @param func Ukazatel do tabulky symbolu na funkci.
 * @param local_table Ukazatel na lokalni tabulku symbolu.
 * @param global_table Ukazatel na globalni tabulku symbolu.
 * @return Typ navratove hodnoty funkce a informace zda muze obsahovat nil.
 */
token_t call_func(global_symtab_t *func, local_symtab_w_par_ptr_t *local_table, global_symtab_t *global_table)
{
    token_t func_out;
    func_out.type = TOK_NOTHING;
    token_t current_token;
    if (strcmp(func->key.data, "write") == 0) // write se zpracovava samostatne kvuli promenlivemu poctu parametru
    {
        getTokenAssert(TOK_L_BRCKT, SYNTAX_ERR);
        while (current_token.type != TOK_R_BRCKT)
        {
            current_token = getTokenAssertArr(6, (token_type_t[]){TOK_INT, TOK_DOUBLE, TOK_STRING, TOK_IDENTIFIER, TOK_R_BRCKT, TOK_KW_NIL}, SYNTAX_ERR);
            if (current_token.type == TOK_R_BRCKT)
            {
                return func_out; // func_out neni definovane v tomto pripade
            }

            if (current_token.type == TOK_KW_NIL)
            {
                gen_value(output, &current_token, false, NULL);
                gen_func_call(output, "write");
                current_token = getTokenAssertArr(2, (token_type_t[]){TOK_COMMA, TOK_R_BRCKT}, SYNTAX_ERR);
                continue;
            }
            else
            if (current_token.type != TOK_IDENTIFIER)
            {
                gen_value(output, &current_token, false, NULL);
                gen_func_call(output, "write");
                current_token = getTokenAssertArr(2, (token_type_t[]){TOK_COMMA, TOK_R_BRCKT}, SYNTAX_ERR);
                continue;
            }
            

            token_t token_out;
            void* var = local_search_in_all(local_table, &current_token.attribute.str);
            if (var == NULL)
            {
                var = global_search(global_table, &current_token.attribute.str);
                if (var == NULL)
                {
                    // error - nedefinovana promenna
                    returnError(VARIABLE_DEFINITION_ERR);
                }
                if (((global_symtab_t*)var)->isInitialised == false)
                {
                    // error - neinicializovana promenna
                    returnError(VARIABLE_DEFINITION_ERR);
                }
                
                token_out.attribute.str = ((global_symtab_t*)var)->key;
                token_out.type = type_t_to_token_type_t(((global_symtab_t*)var)->type);
            }
            else
            {
                if (((local_symtab_t*)var)->isInitialised == false)
                {
                    // error - neinicializovana promenna
                    returnError(VARIABLE_DEFINITION_ERR);
                }
                token_out.attribute.str = ((local_symtab_t*)var)->key;
                token_out.type = ((local_symtab_t*)var)->type;
            }

            token_out.type = kw_to_token_type(token_out.type);

            gen_value(output, NULL, true, token_out.attribute.str.data);
            
            current_token = getTokenAssertArr(2, (token_type_t[]){TOK_COMMA, TOK_R_BRCKT}, SYNTAX_ERR);
            gen_func_call(output, "write");
        }
        return func_out; // func_out neni definovane v tomto pripade
    }
    getTokenAssert(TOK_L_BRCKT, SYNTAX_ERR);

    for (int i = 0; i < func->param_count; i++)
    {
        if (strcmp(func->params[i].name.data, "_") != 0)
        {
            current_token = getTokenAssert(TOK_IDENTIFIER, OTHER_ERR);
            if (dstringCompare(&current_token.attribute.str, &func->params[i].name) != 0)
            {
                returnError(FUNCTION_USAGE_ERR);
                // error - spatne jmeno parametru
            }
            getTokenAssert(TOK_COLON, SYNTAX_ERR);
        }

        current_token = getTokenAssertArr(5, (token_type_t[]){TOK_INT, TOK_DOUBLE, TOK_STRING, TOK_KW_NIL, TOK_IDENTIFIER}, FUNCTION_USAGE_ERR);
        
        if (current_token.type == TOK_IDENTIFIER)
        {
            token_t token_out;
            void* var = local_search_in_all(local_table, &current_token.attribute.str);
            if (var == NULL)
            {
                var = global_search(global_table, &current_token.attribute.str);
                if (var == NULL)
                {
                    // error - nedefinovana promenna
                    returnError(OTHER_ERR);
                }

                token_out.attribute.includesNil = ((global_symtab_t*)var)->includesNil;
                if (token_out.attribute.includesNil != func->params[i].includesNil)
                {
                    // error - spatny typ
                    returnError(FUNCTION_USAGE_ERR);
                }
                //token_out.attribute.str = ((global_symtab_t*)var)->key;
                token_out.type = type_t_to_token_type_t(((global_symtab_t*)var)->type);
            }
            else
            {
                
                token_out.attribute.includesNil = ((local_symtab_t*)var)->includesNil;
                if (token_out.attribute.includesNil != func->params[i].includesNil)
                {
                    // error - spatny typ
                    returnError(FUNCTION_USAGE_ERR);
                }
                token_out.type = ((local_symtab_t*)var)->type;
            }
            token_out.attribute.str = ((local_symtab_t*)var)->key;
            token_out.type = kw_to_token_type(token_out.type);
            if (token_out.type != kw_to_token_type(func->params[i].type))
            {
                // error - spatny typ
                returnError(FUNCTION_USAGE_ERR);
            }
            
            gen_value(output, NULL, true, current_token.attribute.str.data);
        }
        else
        {   
            if (func->params[i].includesNil == true && current_token.type == TOK_KW_NIL) // umoznuje prirazeni nil do parametru
            {
                // pass
            }
            else if (current_token.type != kw_to_token_type(func->params[i].type))
            {
                // error - spatny typ
                returnError(FUNCTION_USAGE_ERR);
            }
            gen_value(output, &current_token, false, NULL);
        }
        

        if (i < func->param_count - 1)
        {
            getTokenAssert(TOK_COMMA, FUNCTION_USAGE_ERR);
            if (getToken().type == TOK_R_BRCKT)
            {
                // error - spatny pocet parametru
                returnError(SYNTAX_ERR);
            }
            ungetToken();
        }
    }
    current_token = getTokenAssertArr(2, (token_type_t[]){TOK_R_BRCKT, TOK_COMMA}, FUNCTION_USAGE_ERR);
    if (current_token.type == TOK_COMMA)
    {
        current_token = getToken();
        if (current_token.type == TOK_R_BRCKT)
        {
            // error - zavorka za carkou
            returnError(SYNTAX_ERR);
        }
        else
        {
            // error - spatny pocet parametru
            returnError(FUNCTION_USAGE_ERR);
        }
        ungetToken();
    }
    
    gen_func_call(output, func->key.data);
    func_out.type = kw_to_token_type(type_t_to_token_type_t(func->type));
    func_out.attribute.includesNil = func->includesNil;
    return func_out;
}

/**
 * @brief Zpracovani nove promenne.
 *
 * @param token_assigner Token, ktery urcuje, zda se jedna o konstantu nebo promennou.
 * @param global_table Ukazatel na globalni tabulku symbolu.
 * @param local_table Ukazatel na lokalni tabulku symbolu.
 * @param nest_level Uroven zanoreni.
 */
void handle_variable(token_t token_assigner, global_symtab_t **global_table, local_symtab_w_par_ptr_t *local_table, int nest_level)
{
    token_t var_type;
    var_type.attribute.includesNil = false;
    var_type.type = TOK_NOTHING;
    token_t func_in;
    bool is_constant = false;

    if (token_assigner.type == TOK_KW_LET) // let uvádí konstantní proměnnou
    {
        is_constant = true;
    }


    // fix aby prochazely testy 4 a 5
    token_t identifier = getTokenAssertArr(2, (token_type_t[]){TOK_IDENTIFIER, TOK_UNDERSCORE}, SYNTAX_ERR);

    if(local_search(local_table->table, &identifier.attribute.str) != NULL || (nest_level == 0 && global_search((*global_table), &identifier.attribute.str) != NULL))
    {
        // error - redefinice promenne
        returnError(FUNCTION_DEFINITION_ERR);
    }

    token_t current_token = getTokenAssertArr(2, (token_type_t[]){TOK_ASSIGN, TOK_COLON}, SYNTAX_ERR);    

    if (current_token.type == TOK_COLON)
    {
        var_type = getTokenAssertArr(3, (token_type_t[]){TOK_KW_DOUBLE, TOK_KW_INT, TOK_KW_STRING}, SYNTAX_ERR);
        var_type.type = kw_to_token_type(var_type.type);
        current_token =  getTokenAssertArr(4, (token_type_t[]){TOK_ASSIGN, TOK_EOF, TOK_EOL, TOK_COMMENT}, SYNTAX_ERR);
        if (current_token.type == TOK_ASSIGN)
        {
            // pass
        }
        else
        {
            if (current_token.type == TOK_EOF)
            {
                ungetToken();
            }
            
            gen_var(output, identifier.attribute.str.data, var_type.attribute.includesNil, identifier.type);

            var_type.type = kw_to_token_type(var_type.type);


            if (var_type.attribute.includesNil == true)
            {   
                if (nest_level == 0)
                {
                    (*global_table) = global_insert((*global_table), &identifier.attribute.str, token_type_t_to_type_t(var_type.type), false, 0, NULL, var_type.attribute.includesNil, is_constant, true);
                }
                else
                {
                    local_table->table = local_insert(local_table->table, &identifier.attribute.str, var_type.type, var_type.attribute.includesNil, is_constant, true);
                }
            }
            else
            {
                if (nest_level == 0)
                {
                    (*global_table) = global_insert((*global_table), &identifier.attribute.str, token_type_t_to_type_t(var_type.type), false, 0, NULL, var_type.attribute.includesNil, is_constant, false);
                }
                else
                {
                    local_table->table = local_insert(local_table->table, &identifier.attribute.str, var_type.type, var_type.attribute.includesNil, is_constant, false);
                }
            }    
            
            return;
        }
    }
    
    current_token = getToken();


    if (current_token.type == TOK_EOF)
    {
        // error - neni prirazeni
        returnError(SYNTAX_ERR);
    }
    if (current_token.type == TOK_EOL) // asi v pohode, neco jako var x = \n 5
    {
        current_token = getToken();
    }
    
    // precti 2. token za =, pak se vrat na 1. token

    bool is_func = false;
    if (current_token.type == TOK_IDENTIFIER)
    {
        if(getToken().type == TOK_L_BRCKT)
        {   
            if((global_search((*global_table), &current_token.attribute.str) != NULL) && (global_search((*global_table), &current_token.attribute.str)->is_func == true))
            {
                is_func = true;
            }
            else
            {
                // error - neni funkce
                returnError(FUNCTION_DEFINITION_ERR);
            }
        }
        ungetToken();
    }

    current_token = getToken();
    ungetToken();
    ungetToken();

    if ((current_token.type != TOK_EOF && current_token.type != TOK_EOL) && is_func == false)
    {
        ast_node_t* node = checkExpression(local_table, (*global_table), false);
        if (node->token.type == TOK_EQUAL || node->token.type == TOK_NOT_EQUAL || node->token.type == TOK_LESSER || node->token.type == TOK_GREATER || node->token.type == TOK_LESSER_OR_EQUAL || node->token.type == TOK_GREATER_OR_EQUAL) {
            // Ak je relacny operator vo vyraze, tak error
            fprintf(stderr, "[PARSER] Relational operator in expression which is not in a condition\n");
            returnError(TYPE_COMPATIBILITY_ERR);
        }
        if (var_type.type == TOK_NOTHING)
        {
            var_type.type = node->type;
        }
        else if (var_type.type == TOK_DOUBLE && node->type == TOK_INT)
        {
            //pass
            if (node->literal == false)
            {
                // error - spatny typ
                returnError(TYPE_COMPATIBILITY_ERR);
            }
        }	
        else if (var_type.type != node->type)
        {
            // error - spatny typ
            returnError(TYPE_COMPATIBILITY_ERR);
        }
        ast_dispose(node);
    }
    else
    {
        current_token = getTokenAssertArr(5, (token_type_t[]){TOK_KW_NIL, TOK_INT, TOK_DOUBLE, TOK_STRING, TOK_IDENTIFIER}, SYNTAX_ERR);
        if (current_token.type == TOK_KW_NIL)
        {
            if (var_type.type == TOK_NOTHING)
            {
                // error - nil do var bez typu
                returnError(TYPE_DEDUCTION_ERR);
            }
            if (var_type.attribute.includesNil == false)
            {
                // error - prirazeni nil do non-nil var
                returnError(TYPE_COMPATIBILITY_ERR);
            }
            var_type.type = kw_to_token_type(var_type.type);
            gen_value(output, &current_token, false, NULL);
        }
        else if (current_token.type == TOK_IDENTIFIER) // prirazeni promenne nebo volani funkce
        {
            local_symtab_t* var_loc = local_search_in_all(local_table, &current_token.attribute.str);
            if (var_loc == NULL)
            {
                global_symtab_t* var_glob = (global_symtab_t*)global_search((*global_table), &current_token.attribute.str);
                if (var_glob == NULL)
                {
                    if (getToken().type == TOK_L_BRCKT)
                    {
                        // error - nedefinovana funkce
                        returnError(FUNCTION_DEFINITION_ERR);
                    }
                    else
                    {
                        // error - nedefinovana promenna
                        returnError(VARIABLE_DEFINITION_ERR);
                    }
                }
                if (var_glob->is_func == false)
                {
                    var_type.type = kw_to_token_type(var_type.type);

                    if (var_glob->isInitialised == false)
                    {
                        // error - neinicializovana promenna
                        returnError(VARIABLE_DEFINITION_ERR);
                    }

                    if(var_type.type == TOK_NOTHING)
                    {
                        var_glob->type = var_glob->type;
                    }
                    else if ((var_glob->type == T_INT && var_type.type != TOK_INT) || (var_glob->type == T_DOUBLE && var_type.type != TOK_DOUBLE) || (var_glob->type == T_STRING && var_type.type != TOK_STRING))
                    {
                        // error - spatny typ
                        returnError(TYPE_COMPATIBILITY_ERR);
                    }
                    
                    gen_value(output, &var_type, true, current_token.attribute.str.data);
                }
                else
                {
                    if (var_glob->type == T_NIL)
                    {
                        // error - nil do var bez typu
                        returnError(TYPE_COMPATIBILITY_ERR);
                    }
                    
                    func_in = call_func(var_glob, local_table, (*global_table));
                    var_type.attribute.includesNil = func_in.attribute.includesNil;
                    var_type.type = func_in.type;
                }
            }
            else
            {
                var_type.type = kw_to_token_type(var_type.type);

                if(var_type.type == TOK_NOTHING)
                {
                    var_type.type = var_loc->type;
                }
                else if (var_loc->type != var_type.type)
                {
                    // error - spatny typ
                    returnError(TYPE_COMPATIBILITY_ERR);
                }
                gen_value(output, &var_type, true, current_token.attribute.str.data);
            }

        }
        else // prirazeni literalu
        {
            var_type.type = kw_to_token_type(var_type.type);
            if (var_type.type == TOK_NOTHING)
            {
                var_type.type = current_token.type;
            }
            else if (current_token.type != var_type.type && !(current_token.type == TOK_INT && var_type.type == TOK_DOUBLE)) // prirazeni int do double je legal
            {
                //error - spatny typ
                returnError(TYPE_COMPATIBILITY_ERR);
            }
            gen_value(output, &current_token, false, NULL);
        }
    }
    
    
    gen_var(output, identifier.attribute.str.data, var_type.attribute.includesNil, identifier.type);
    gen_assign(output, identifier.attribute.str.data, var_type.type);

    current_token = getToken();
    if (current_token.type == TOK_EOF)
    {
        token_table.index--;
    }
    
    var_type.type = kw_to_token_type(var_type.type);
    
    
    if (nest_level == 0)
    {
        (*global_table) = global_insert((*global_table), &identifier.attribute.str, token_type_t_to_type_t(var_type.type), false, 0, NULL, var_type.attribute.includesNil, is_constant, true);
    }
    else
    {
        local_table->table = local_insert(local_table->table, &identifier.attribute.str, var_type.type, var_type.attribute.includesNil, is_constant, true);
    }
}

/**
 * @brief Prirazeni hodnoty do promenne/volani funkce.
 *
 * @param token_id Nazev promenne ci funkce.
 * @param global_table Ukazatel na globalni tabulku symbolu.
 * @param local_table Ukazatel na lokalni tabulku symbolu.
 * @param nest_level Uroven zanoreni.
 */
void handle_assign_or_call_func(token_t token_id, global_symtab_t *global_table, local_symtab_w_par_ptr_t *local_table, int nest_level)
{
    token_t current_token = getToken();
    if (current_token.type == TOK_L_BRCKT) // za id ihned '('
    {
        ungetToken();
        global_symtab_t* func = global_search(global_table, &token_id.attribute.str);
        if (func == NULL || func->is_func == false) // pokud neni funkce
        {
            if (func == NULL)
            {
                // error - nedefinovana funkce
                returnError(FUNCTION_DEFINITION_ERR);
            }
            else
            {
                // error - neni funkce
                returnError(FUNCTION_DEFINITION_ERR);
            }
        }
        call_func(func, local_table, global_table);
    }
    else if (current_token.type == TOK_ASSIGN)
    {
        token_type_t var_type;
        void* var = local_search_in_all(local_table, &token_id.attribute.str);
        if (var == NULL)
        {   
            var = global_search(global_table, &token_id.attribute.str);
            if (var == NULL)
            {
                // error - nedefinovana promenna
                returnError(VARIABLE_DEFINITION_ERR);
            }
            var_type = type_t_to_token_type_t(((global_symtab_t*)var)->type);
            if ((((global_symtab_t*)var)->isConstant == true && ((global_symtab_t*)var)->isInitialised == true) || ((global_symtab_t*)var)->is_func == true)
            {
                // error - prirazeni do konstanty
                returnError(OTHER_ERR);
            }
            if (nest_level == 0)
            {
                ((global_symtab_t*)var)->isInitialised = true;
            }
        }
        else
        {
            var_type = ((local_symtab_t*)var)->type;
            if (((local_symtab_t*)var)->isConstant == true && ((local_symtab_t*)var)->isInitialised == true)
            {
                // error - prirazeni do konstanty
                returnError(OTHER_ERR);
            }
            if (nest_level == 0 || local_search(local_table->table, &token_id.attribute.str) != NULL)
            {
                ((local_symtab_t*)var)->isInitialised = true;
            }
        }

        bool is_func = false;
        current_token = getToken();
        if (current_token.type == TOK_IDENTIFIER)
        {
            if(getToken().type == TOK_L_BRCKT)
            {   
                if((global_search(global_table, &current_token.attribute.str) != NULL) && (global_search(global_table, &current_token.attribute.str)->is_func == true))
                {
                    is_func = true;
                }
                else
                {
                    // error - neni funkce
                    returnError(FUNCTION_DEFINITION_ERR);
                }
            }
            ungetToken();
        }
        else
        {
            //getToken();
        }

        current_token = getToken();
        ungetToken();
        ungetToken();
        
        if ((current_token.type != TOK_EOF && current_token.type != TOK_EOL) && is_func == false)
        {
            ast_node_t* node = checkExpression(local_table, global_table, false);
            if (var_type == TOK_NOTHING)
            {
                var_type = node->type;
            }
            else if (var_type == TOK_DOUBLE && node->type == TOK_INT)
            {
                //pass
                if (node->literal == false)
                {
                    // error - spatny typ
                    returnError(TYPE_COMPATIBILITY_ERR);
                }
            }	
            else if (var_type != node->type)
            {
                // error - spatny typ
                returnError(TYPE_COMPATIBILITY_ERR);
            }
            ast_dispose(node);
        }
        else
        {
            current_token = getToken();
            if (current_token.type != TOK_IDENTIFIER)
            {
                {
                    ungetToken();
                }

                if (current_token.type == TOK_INT && var_type == TOK_DOUBLE)
                {
                    // pass, pretypovani int na double
                }
                else if (current_token.type != var_type)
                {
                    // error - spatny typ
                    returnError(TYPE_COMPATIBILITY_ERR);
                }
                gen_value(output, &current_token, false, NULL);   
            }
            else if (global_search(global_table, &current_token.attribute.str) != NULL)
            {
                if(global_search(global_table, &current_token.attribute.str)->isInitialised == false)
                {
                    // error - neinicializovana promenna
                    returnError(VARIABLE_DEFINITION_ERR);
                }
                if(global_search(global_table, &current_token.attribute.str)->is_func == true)
                {
                    token_t func_in = call_func(global_search(global_table, &current_token.attribute.str), local_table, global_table);
                    if (func_in.type != var_type)
                    {
                        // error - spatny typ
                        returnError(TYPE_COMPATIBILITY_ERR);
                    }
                }
                else
                {
                    if (global_search(global_table, &current_token.attribute.str)->type != token_type_t_to_type_t(var_type))
                    {
                        // error - spatny typ
                        returnError(TYPE_COMPATIBILITY_ERR);
                    }
                    current_token.type = type_t_to_token_type_t(global_search(global_table, &current_token.attribute.str)->type);
                    gen_value(output, &current_token, true, current_token.attribute.str.data);
                }
            }
            else
            {
                // error - nedefinovana funkce
                returnError(VARIABLE_DEFINITION_ERR);
            }
        }

        gen_assign(output, token_id.attribute.str.data, var_type);
    }
    else
    {
        // error
        returnError(SYNTAX_ERR);
    }
}


/**
 * @brief Zpracovani definice funkce.
 *
 * @param global_table Ukazatel na globalni tabulku symbolu.
 * @param local_table_one_up Ukazatel na lokalni tabulku symbolu.
 */
void handle_func_def(global_symtab_t *global_table, local_symtab_w_par_ptr_t *local_table_one_up)
{
    token_t token = getTokenAssert(TOK_IDENTIFIER, SYNTAX_ERR);
    global_symtab_t *found = global_search(global_table, &token.attribute.str);
    if (found == NULL)
    {
        // error - nevim jak by se stalo, mozna neni potreba
        returnError(FUNCTION_DEFINITION_ERR);
    }
    if (found->is_func == false)
    {
        // error - neni funkce
        returnError(FUNCTION_DEFINITION_ERR);
    }
    
    if (strcmp(found->key.data, "write")        == 0
    ||  strcmp(found->key.data, "readString")   == 0
    ||  strcmp(found->key.data, "readInt")      == 0
    ||  strcmp(found->key.data, "readDouble")   == 0
    ||  strcmp(found->key.data, "Int2Double")   == 0
    ||  strcmp(found->key.data, "Double2Int")   == 0
    ||  strcmp(found->key.data, "length")       == 0
    ||  strcmp(found->key.data, "substring")    == 0
    ||  strcmp(found->key.data, "ord")          == 0
    ||  strcmp(found->key.data, "chr")          == 0)
    {
        // toto nepassovat do generatoru - built-in funkce
    }
    else
    {
        gen_func(output, &token);
    }
    // spravnost definice se uz kontroluje v 1. pruchodu
    getTokenAssert(TOK_L_BRCKT, SYNTAX_ERR);
    while (getToken().type != TOK_R_BRCKT)
    {
        ;
    }
    if (getToken().type == TOK_ARROW)
    {
        getTokenAssertArr(3, (token_type_t[]){TOK_KW_DOUBLE, TOK_KW_INT, TOK_KW_STRING}, SYNTAX_ERR);
    }
    else
    {
        ungetToken();
    }
    token_t current_token = getTokenAssertArr(2, (token_type_t[]){TOK_L_CRL_BRCKT, TOK_EOL}, SYNTAX_ERR);
    if (current_token.type == TOK_EOL)
    {
        current_token = getTokenAssert(TOK_L_CRL_BRCKT, SYNTAX_ERR);
    }
    
    local_symtab_w_par_ptr_t local_table;
    local_init_w_par_ptr_t(&local_table);
    local_table.parent = local_table_one_up;


    for (int i = 0; i < found->param_count; i++)
    {
        local_table.table = local_insert(local_table.table, &found->params[i].identifier, kw_to_token_type(found->params[i].type), found->params[i].includesNil, true, true); // paramentry nelze menit uvnitr funkce, asi?
    }

    if (strcmp(found->key.data, "write")        == 0
    ||  strcmp(found->key.data, "readString")   == 0
    ||  strcmp(found->key.data, "readInt")      == 0
    ||  strcmp(found->key.data, "readDouble")   == 0
    ||  strcmp(found->key.data, "Int2Double")   == 0
    ||  strcmp(found->key.data, "Double2Int")   == 0
    ||  strcmp(found->key.data, "length")       == 0
    ||  strcmp(found->key.data, "substring")    == 0
    ||  strcmp(found->key.data, "ord")          == 0
    ||  strcmp(found->key.data, "chr")          == 0)
    {
        // toto nepassovat do generatoru - built-in funkce
        getTokenAssert(TOK_R_CRL_BRCKT, SYNTAX_ERR);
    }
    else
    {
        if (local_table.table != NULL){
            for (int i = 0; i < found->param_count; i++){
                gen_var(output, found->params[i].identifier.data, found->params[i].includesNil, kw_to_token_type(found->params[i].type));
            }
        }
        if (local_table.table != NULL){
            for (int i = found->param_count - 1; i >= 0; i--){
                gen_assign(output, found->params[i].identifier.data, kw_to_token_type(found->params[i].type)); 
            }
        }
        parse_block(-MAX_NEST_LEVEL, TOK_L_CRL_BRCKT, &global_table, &local_table, NULL, TOK_NOTHING, type_t_to_token_type_t(found->type));
        gen_func_end(output, &token);
    }
}

/**
 * @brief Zpracovani podminky
 *
 * @param local_table Ukazatel na lokalni tabulku symbolu.
 * @param global_table Ukazatel na globalni tabulku symbolu.
 */
void handle_cond(local_symtab_w_par_ptr_t *local_table, global_symtab_t *global_table)
{
    ast_dispose(checkExpression(local_table, global_table, true));
}

/**
 * @brief Zpracovani podminky if.
 *
 * @param nest_level Uroven zanoreni.
 * @param local_table Ukazatel na lokalni tabulku symbolu.
 * @param global_table Ukazatel na globalni tabulku symbolu.
 */
void handle_if(int nest_level, local_symtab_w_par_ptr_t *local_table, global_symtab_t *global_table)
{
    token_t current_token = getToken();

    if (current_token.type == TOK_EOL)
    {
        current_token = getToken();
    }

    if (current_token.type == TOK_KW_LET)
    {
        current_token = getTokenAssert(TOK_IDENTIFIER, SYNTAX_ERR);
        void* var = local_search_in_all(local_table, &current_token.attribute.str);
        if(var == NULL)
        {
            var = global_search(global_table, &current_token.attribute.str);
            if (var == NULL)
            {
                // error - nedefinovana promenna
                returnError(VARIABLE_DEFINITION_ERR);
            }
            gen_if_let(output, ((global_symtab_t*)var)->key.data);
            gen_if(output);
            if (((global_symtab_t*)var)->includesNil == false)
            {
                // error - non-nil promenna v if-let
                returnError(OTHER_ERR);
            }
            if (getToken().type != TOK_EOL)
            {
                ungetToken();
            }
            
            getTokenAssert(TOK_L_CRL_BRCKT, SYNTAX_ERR);
            ignore_comments();
            parse_block(nest_level + 1, TOK_L_CRL_BRCKT, &global_table, local_table, &((global_symtab_t*)var)->key, type_t_to_token_type_t(((global_symtab_t*)var)->type), TOK_NOTHING);
        }
        else
        {
            gen_if_let(output, ((local_symtab_t*)var)->key.data);
            gen_if(output);
            if (((local_symtab_t*)var)->includesNil == false)
            {
                // error - non-nil promenna v if-let
                returnError(OTHER_ERR);
            }            
            if (getToken().type != TOK_EOL)
            {
                ungetToken();
            }
            getTokenAssert(TOK_L_CRL_BRCKT, SYNTAX_ERR);
            ignore_comments();
            parse_block(nest_level + 1, TOK_L_CRL_BRCKT, &global_table, local_table, &((local_symtab_t*)var)->key, ((local_symtab_t*)var)->type, TOK_NOTHING);
        }
    }
    else
    {
        ungetToken();
        handle_cond(local_table, global_table);
        gen_if(output);
        if (getToken().type != TOK_EOL)
        {
            ungetToken();
        }
        getTokenAssert(TOK_L_CRL_BRCKT, SYNTAX_ERR);
        ignore_comments();
        parse_block(nest_level + 1, TOK_L_CRL_BRCKT, &global_table, local_table, NULL, TOK_NOTHING, TOK_NOTHING);
    }
    
    current_token = getToken();

    if(current_token.type == TOK_EOL)
    {
        current_token = getToken();
    }

    if (current_token.type == TOK_KW_ELSE)
    {
        if (getToken().type != TOK_EOL)
        {
            ungetToken();
        }
        getTokenAssert(TOK_L_CRL_BRCKT, SYNTAX_ERR);
        ignore_comments();
        gen_else(output);
        parse_block(nest_level + 1, TOK_L_CRL_BRCKT, &global_table, local_table, NULL, TOK_NOTHING, TOK_NOTHING);
    }
    else
    {
        ungetToken();
    }
    gen_if_end(output);
}

/**
 * @brief Zpracovani podminky while.
 *
 * @param nest_level Uroven zanoreni.
 * @param local_table Ukazatel na lokalni tabulku symbolu.
 * @param global_table Ukazatel na globalni tabulku symbolu.
 */
void handle_while(int nest_level, local_symtab_w_par_ptr_t *local_table, global_symtab_t *global_table)
{
    gen_while(output);
    handle_cond(local_table, global_table);
    gen_while_body(output);
    if (getToken().type != TOK_EOL)
    {
        ungetToken();
    }
    getTokenAssert(TOK_L_CRL_BRCKT, VARIABLE_DEFINITION_ERR);
    ignore_comments();
    parse_block(nest_level + 1, TOK_L_CRL_BRCKT, &global_table, local_table, NULL, TOK_NOTHING, TOK_NOTHING);
    gen_while_end(output);
}

/**
 * @brief Zpracovani bloku kodu.
 *
 * @param nest_level Uroven zanoreni.
 * @param block_start Token, ktery urcuje zda se jedna o blok v {} nebo ().
 * @param global_table Ukazatel na globalni tabulku symbolu.
 * @param local_table_one_up Ukazatel na lokalni tabulku symbolu.
 * @param var_name Nazev promenne, ktera se ma na zacatku vlozit do tabulky symbolu.
 * @param var_type Typ promenne, ktera se ma na zacatku vlozit do tabulky symbolu.
 * @param return_type Predpokladany typ navratove hodnoty.
 * @return Udava zda blok obsahuje return.
 */
bool parse_block(int nest_level, token_type_t block_start, global_symtab_t **global_table, local_symtab_w_par_ptr_t *local_table_one_up, string_t *var_name, token_type_t var_type, token_type_t expected_return) // returnuje posledni token
{
    bool has_return = false;
    token_type_t block_end;
    if (block_start == TOK_L_BRCKT)
    {
        block_end = TOK_R_BRCKT;
    }
    else if (block_start == TOK_L_CRL_BRCKT)
    {
        block_end = TOK_R_CRL_BRCKT;
    }
    else if (block_start == TOK_NOTHING)
    {
        block_end = TOK_EOF;
    }
    else
    {
        // error - neznamy typ bloku
        returnError(SYNTAX_ERR);
    }

    token_t current_token;
    current_token = getToken();


    local_symtab_w_par_ptr_t local_table;
    local_init_w_par_ptr_t(&local_table);
    
    if (var_name != NULL)
    {
        local_symtab_w_par_ptr_t local_table_tmp;
        local_init_w_par_ptr_t(&local_table_tmp);
        local_table_tmp.table = local_insert(local_table_tmp.table, var_name, var_type, false, true, true);
        local_table_tmp.parent = local_table_one_up;
        local_table.parent = &local_table_tmp;
    }
    else
    {
        local_table.parent = local_table_one_up; // checknout jeste
    }


    // nest levels:
    // main             0
    // podblok mainu    1+
    // funkce           -MAX_NEST_LEVEL...-1
    if(nest_level%MAX_NEST_LEVEL == - 1)
    {
        // error - max nest level
        returnError(INTERN_ERR);
    }

    while (token_table.index < token_table.size && current_token.type != block_end) {
        if (current_token.type == TOK_COMMENT)
        {
            ;
        }
        else if (current_token.type == TOK_BLOCK_COM_START)
        {
            getTokenAssert(TOK_BLOCK_COM_END, SYNTAX_ERR);
        }
        else if (current_token.type == TOK_L_CRL_BRCKT)
        {
            parse_block(nest_level + 1, TOK_L_CRL_BRCKT, &(*global_table), &local_table, NULL, TOK_NOTHING, TOK_NOTHING);
        }
        else if (current_token.type == TOK_R_CRL_BRCKT)
        {
            // error
            returnError(SYNTAX_ERR);
        }
        else if (current_token.type == TOK_KW_LET || current_token.type == TOK_KW_VAR)
        {
            handle_variable(current_token, &(*global_table), &local_table, nest_level);
        }
        else if (current_token.type == TOK_IDENTIFIER)
        {
            handle_assign_or_call_func(current_token, (*global_table), &local_table, nest_level);
        }
        else if (current_token.type == TOK_KW_FUNC)
        {
            if (nest_level != 0)
            {
                // error - funkce definovana v podbloku
                returnError(OTHER_ERR);
            }
            else
            {
                handle_func_def((*global_table), &local_table);
            }
        }
        else if (current_token.type == TOK_KW_IF)
        {
            handle_if(nest_level, &local_table, (*global_table));
        }
        else if (current_token.type == TOK_KW_WHILE)
        {
            handle_while(nest_level, &local_table, (*global_table));
        }
        else if (current_token.type == TOK_KW_RETURN)
        {
            has_return = true;
            if (nest_level >= 0)
            {
                // error - return v mainu, asi error
                returnError(SYNTAX_ERR);
            }
            if (nest_level == -MAX_NEST_LEVEL)
            {
                current_token = getToken();
                if (current_token.type != TOK_EOL && current_token.type != TOK_EOF)
                {
                    ungetToken();
                    ast_node_t* node = checkExpression(&local_table, (*global_table), false);
                    token_type_t return_type = node->type;
                    ast_dispose(node);
                    if (return_type == TOK_NOTHING)
                    {
                        // error - spatny typ
                        returnError(SYNTAX_ERR);
                    }
                    if (return_type != expected_return)
                    {
                        if (expected_return == TOK_NOTHING)
                        {
                            returnError(FUNCTION_RETURN_ERROR);
                        }
                        returnError(FUNCTION_USAGE_ERR);
                    }
                }
                else
                {
                    if (expected_return != TOK_NOTHING)
                    {
                        // error - spatny typ
                        returnError(FUNCTION_RETURN_ERROR);
                    }
                }
                gen_func_return(output);
            }
        }
        current_token = getToken();
    }
    local_dispose(&local_table.table);
    return has_return;
}


/**
 * @brief Precte podblok, ignoruje obsah.
 *
 * @param token Token, ktery urcuje zda se jedna o blok v {} nebo ().
 */
void read_subblock(token_t token)
{
    token_t block_end;
    if (token.type == TOK_L_BRCKT)
    {
        block_end.type = TOK_R_BRCKT;
    }
    else if (token.type == TOK_L_CRL_BRCKT)
    {
        block_end.type = TOK_R_CRL_BRCKT;
    }
    else
    {
        // error
        returnError(SYNTAX_ERR);
    }

    token_t current_token = getToken();
    while (current_token.type != block_end.type)
    {
        if (current_token.type == TOK_EOF)
        {
            returnError(SYNTAX_ERR);
        }
        else if (current_token.type == TOK_COMMENT)
        {
            ;
        }
        else if (current_token.type == TOK_BLOCK_COM_START)
        {
            getTokenAssert(TOK_BLOCK_COM_END, SYNTAX_ERR);
        } 
        else if (current_token.type == TOK_L_CRL_BRCKT || current_token.type == TOK_L_BRCKT)
        {
            read_subblock(current_token);
        }
        else if (current_token.type == TOK_R_CRL_BRCKT || current_token.type == TOK_R_BRCKT) // (...} nebo {...)
        {
            // error - uzaviraci zavorka bud volne, nebo nasledujici spatny typ zavorky
            returnError(SYNTAX_ERR);
        }
        current_token = getToken();
    }
    return;
}

/**
 * @brief Najde funkce v kodu, ulozi je to globalni tabulky symbolu.
 *
 * @param global_table Ukazatel na globalni tabulku symbolu.
 */
void find_functions(global_symtab_t **global_table)
{
    token_t current_token;
    current_token = getToken();

    while (current_token.type != TOK_EOF) {
        if (current_token.type == TOK_COMMENT)
        {
            ;
        }
        else if (current_token.type == TOK_BLOCK_COM_START)
        {
            getTokenAssert(TOK_BLOCK_COM_END, SYNTAX_ERR);
        }
        
        else if (current_token.type == TOK_L_CRL_BRCKT || current_token.type == TOK_L_BRCKT)
        {
            read_subblock(current_token);
        }
        else if (current_token.type == TOK_R_CRL_BRCKT || current_token.type == TOK_R_BRCKT)
        {
            // error
            returnError(SYNTAX_ERR);
        }
        else if (current_token.type == TOK_KW_FUNC) // neresim kde je definovana, jen ze ne v podbloku
        {
            token_t func_name = getTokenAssert(TOK_IDENTIFIER, SYNTAX_ERR);

            global_symtab_t* func = global_search(*global_table, &func_name.attribute.str);
            if (func != NULL)
            {
                // error - redefinice
                returnError(OTHER_ERR);
            }
            current_token = getTokenAssert(TOK_L_BRCKT, SYNTAX_ERR);
            func_param_t *params = NULL;
            int param_cntr = 0;
            current_token = getToken();
            while (current_token.type != TOK_R_BRCKT)
            {
                token_t name = current_token;
                if (name.type == TOK_UNDERSCORE)
                {
                    // pass, _ je validni jmeno
                }
                else if (name.type != TOK_IDENTIFIER)
                {
                    // error
                    returnError(SYNTAX_ERR);
                }
                
                token_t id = getToken();
                if (id.type == TOK_UNDERSCORE)
                {
                    // pass, umoznuje passovat nevyuzite parametry
                }
                else if (id.type != TOK_IDENTIFIER)
                {
                    // error
                    returnError(SYNTAX_ERR);
                }


                if (dstringCompare(&name.attribute.str, &id.attribute.str) == 0 && strcmp(name.attribute.str.data, "_") != 0)
                {
                    // error - jmeno a id se nesmi rovnat
                    returnError(OTHER_ERR); // asi 9?
                }

                getTokenAssert(TOK_COLON, TYPE_DEDUCTION_ERR);

                current_token = getTokenAssertArr(3, (token_type_t[]){TOK_KW_DOUBLE, TOK_KW_INT, TOK_KW_STRING}, SYNTAX_ERR);

                if (param_cntr == 0)
                {
                    params = malloc(sizeof(func_param_t));
                    if (params == 0)
                    {
                        // error - malloc fail
                        returnError(INTERN_ERR);
                    }
                }
                else
                { 
                    func_param_t* tmp = realloc(params, sizeof(func_param_t) * (param_cntr + 1)); // neni idealni, ale asi nebude tolik paramentru aby na tom zalezelo
                    if (tmp == 0)
                    {
                        // error - realloc fail
                        returnError(INTERN_ERR);
                    }
                    params = tmp;
                }

                params[param_cntr].name = name.attribute.str;
                params[param_cntr].identifier = id.attribute.str;
                params[param_cntr].type = current_token.type;
                params[param_cntr].includesNil = current_token.attribute.includesNil;

                current_token = getTokenAssertArr(2, (token_type_t[]){TOK_COMMA, TOK_R_BRCKT}, SYNTAX_ERR);

                if (current_token.type == TOK_COMMA)
                {
                    current_token = getToken();
                }

                param_cntr++;
            }

            current_token = getToken();
            if (current_token.type == TOK_ARROW)
            {
                current_token = getTokenAssertArr(3, (token_type_t[]){TOK_KW_DOUBLE, TOK_KW_INT, TOK_KW_STRING}, SYNTAX_ERR);
                type_t current_token_type;
                current_token_type = token_type_t_to_type_t(kw_to_token_type(current_token.type));
                
                *global_table = global_insert(*global_table, &func_name.attribute.str, current_token_type, true, param_cntr, params, current_token.attribute.includesNil, true, true);
            }
            else
            {
                *global_table = global_insert(*global_table, &func_name.attribute.str, T_NIL, true, param_cntr, params, current_token.attribute.includesNil, true, true); // void funkce
                continue;
            }
            
            // obsah uvnitr funkce resim v 2. pruchodu
        }
        else if (current_token.type == TOK_UNDERSCORE)
        {
            // error - _ neni validni identifikator
            returnError(SYNTAX_ERR);
        }
        current_token = getToken();
    }
    current_token.type = TOK_EOF;
    addTableToken(&token_table, current_token);
}

/**
 * @brief Hlavni funkce parseru.
 *
 */
int parse()
{
    global_symtab_t *global_table;
    global_init(&global_table);
    load_built_in_functions();
    initTokenTable(&token_table);
    find_functions(&global_table);
    token_table.insert = false;
    token_table.index = 0;
    parse_block(0, TOK_NOTHING, &global_table, NULL, NULL, TOK_NOTHING, TOK_NOTHING);
    freeTokenTable(&token_table);
    global_dispose(&global_table);
    return 1;
}