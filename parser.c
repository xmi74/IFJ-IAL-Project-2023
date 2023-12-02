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
extern int counter;

#define MAX_NEST_LEVEL 1000

// TODO: inicializace promennych
// TODO: promenne do global symtab v mainu
// TODO: returns + codegen?
// TODO: mozna assignment do funkci, e.g. write = 5
// TODO: volani generatoru pri var x = y, apod., jak pushovat hodnotu?

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
        // TODO
        return TOK_NOTHING;
    }
}

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
        // TODO
        return T_NIL;
    }
}


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
        // TODO
        return kw;
    }
}

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
    "func length(_ s : String) -> Int {}\n"
    "func ord(_ c : String) -> Int {}\n"
    "func chr(_ i : Int) -> String {}\n");

    for (int i = strlen(funcs)-1; i >= 0; i--)
    {
        ungetc(funcs[i], stdin);
    }
}

void call_func(global_symtab_t *func, local_symtab_w_par_ptr_t *local_table, global_symtab_t *global_table)
{
    token_t current_token;
    if (strcmp(func->key.data, "write") == 0)
    {
        getTokenAssert(TOK_L_BRCKT);
        while (current_token.type != TOK_R_BRCKT)
        {
            current_token = getTokenAssertArr(4, (token_type_t[]){TOK_INT, TOK_DOUBLE, TOK_STRING, TOK_IDENTIFIER}); // TODO: muze se printovat nil?, ve swiftu ne

            if (current_token.type != TOK_IDENTIFIER)
            {
                gen_value(output, &current_token, false, NULL);
                current_token = getTokenAssertArr(2, (token_type_t[]){TOK_COMMA, TOK_R_BRCKT});
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
                

                if (((global_symtab_t*)var)->type == T_INT)
                {
                    token_out.type = TOK_INT;
                }
                else if (((global_symtab_t*)var)->type == T_DOUBLE)
                {
                    token_out.type = TOK_DOUBLE;
                }
                else if (((global_symtab_t*)var)->type == T_STRING)
                {
                    token_out.type = TOK_STRING;
                }
                else
                {
                    // TODO
                }
            }
            else
            {
                if (((local_symtab_t*)var)->isInitialised == false)
                {
                    // error - neinicializovana promenna
                    returnError(VARIABLE_DEFINITION_ERR);
                }
                token_out.type = ((local_symtab_t*)var)->type;
            }

            token_out.type = kw_to_token_type(token_out.type);
            // TODO
            

            gen_value(output, &token_out, current_token.attribute.str.data, NULL);
            current_token = getTokenAssertArr(2, (token_type_t[]){TOK_COMMA, TOK_R_BRCKT});
        }
        gen_func_call(output, func->key.data);
        return;
    }
    getTokenAssert(TOK_L_BRCKT);

    for (int i = 0; i < func->param_count; i++)
    {
        if (strcmp(func->params[i].name.data, "_") != 0)
        {
            current_token = getTokenAssert(TOK_IDENTIFIER);
            if (dstringCompare(&current_token.attribute.str, &func->params[i].name) != 0)
            {
                returnError(FUNCTION_USAGE_ERR);
                // error - spatne jmeno parametru
            }
            getTokenAssert(TOK_COLON);
        }

        current_token = getTokenAssertArr(5, (token_type_t[]){TOK_INT, TOK_DOUBLE, TOK_STRING, TOK_KW_NIL, TOK_IDENTIFIER});
        
        if (current_token.type == TOK_IDENTIFIER)
        {
            token_t token_out;
            void* var = local_search_in_all(local_table, &current_token.attribute.str);
            if (var == NULL)
            {
                var = global_search(global_table, &current_token.attribute.str);
                if (var == NULL)
                {
                    returnError(VARIABLE_DEFINITION_ERR);
                    // error - nedefinovana promenna
                }
                if (((global_symtab_t*)var)->type == T_INT)
                {
                    token_out.type = TOK_INT;
                }
                else if (((global_symtab_t*)var)->type == T_DOUBLE)
                {
                    token_out.type = TOK_DOUBLE;
                }
                else if (((global_symtab_t*)var)->type == T_STRING)
                {
                    token_out.type = TOK_STRING;
                }
                else
                {
                    // TODO
                }
            }
            else
            {
                token_out.type = ((local_symtab_t*)var)->type;
            }
            token_out.type = kw_to_token_type(token_out.type);
            gen_value(output, &token_out, true, current_token.attribute.str.data);
        }
        else
        {
            gen_value(output, &current_token, false, NULL);
        }

        if (i < global_table->param_count - 1)
        {
            getTokenAssert(TOK_COMMA);
        }
    }
    getTokenAssert(TOK_R_BRCKT);
    gen_func_call(output, func->key.data);
}

void handle_variable(token_t token_assigner, global_symtab_t *global_table, local_symtab_w_par_ptr_t *local_table, int nest_level)
{
    token_t var_type;
    var_type.type = TOK_NOTHING;
    var_type.attribute.includesNil = false;
    bool is_constant = false;

    if (token_assigner.type == TOK_KW_LET) // let uvádí konstantní proměnnou
    {
        is_constant = true;
    }

    token_t identifier = getTokenAssert(TOK_IDENTIFIER);

    if(local_search(local_table->table, &identifier.attribute.str) != NULL)
    {
        // error - redefinice promenne
        returnError(FUNCTION_DEFINITION_ERR);
    }

    token_t current_token = getTokenAssertArr(2, (token_type_t[]){TOK_ASSIGN, TOK_COLON});    

    if (current_token.type == TOK_COLON)
    {
        var_type = getTokenAssertArr(3, (token_type_t[]){TOK_KW_DOUBLE, TOK_KW_INT, TOK_KW_STRING});
        current_token =  getTokenAssertArr(3, (token_type_t[]){TOK_ASSIGN, TOK_EOF, TOK_EOL});
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
            
            if (nest_level == 0)
            {
                gen_var(output, &identifier);
            }
            else
            {
                gen_var(output, &identifier);
            }

            var_type.type = kw_to_token_type(var_type.type);

            if (nest_level == 0)
            {
                global_table = global_insert(global_table, &identifier.attribute.str, token_type_t_to_type_t(var_type.type), false, 0, NULL, var_type.attribute.includesNil, is_constant, false);
            }
            else
            {
                local_table->table = local_insert(local_table->table, &identifier.attribute.str, var_type.type, var_type.attribute.includesNil, is_constant, false);
            }
            
            return;
        }
    }
    if (nest_level == 0)
    {
        gen_var(output, &identifier);
    }
    else
    {
        gen_var(output, &identifier);
    }
    
    current_token = getToken();


    // TODO: tohle upravit, + volani expr

    if (current_token.type == TOK_EOF)
    {
        // error - neni prirazeni
        returnError(SYNTAX_ERR);
    }
    if (current_token.type == TOK_EOL) // asi v pohode, neco jako var x = \n 5
    {
        getToken();
    }
    
    // precti 2. token za =, pak se vrat na 1. token
    current_token = getToken();

    ungetToken();
    ungetToken();

    if (current_token.type != TOK_EOF && current_token.type != TOK_EOL)
    {
        token_type_t type_expr = checkExpression(local_table, global_table);
        if (var_type.type == TOK_NOTHING)
        {
            var_type.type = type_expr;
        }
        else if (var_type.type != type_expr)
        {
            // error - spatny typ
            returnError(TYPE_COMPATIBILITY_ERR);
        }
    }
    else
    {
        current_token = getTokenAssertArr(5, (token_type_t[]){TOK_KW_NIL, TOK_INT, TOK_DOUBLE, TOK_STRING, TOK_IDENTIFIER});
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
        }
        else if (current_token.type == TOK_IDENTIFIER)
        {
            local_symtab_t* var_loc = local_search_in_all(local_table, &current_token.attribute.str);
            if (var_loc == NULL)
            {
                global_symtab_t* var_glob = (global_symtab_t*)global_search(global_table, &current_token.attribute.str);
                if (var_glob == NULL)
                {
                    // error - nedefinovana funkce
                    returnError(FUNCTION_DEFINITION_ERR);
                }
                if (var_glob->is_func == false)
                {
                    var_type.type = kw_to_token_type(var_type.type);

                    if(var_type.type == TOK_NOTHING)
                    {
                        if (var_type.type == TOK_INT)
                        {
                            var_glob->type = T_INT;
                        }
                        else if (var_type.type == TOK_DOUBLE)
                        {
                            var_glob->type = T_DOUBLE;
                        }
                        else if (var_type.type == TOK_STRING)
                        {
                            var_glob->type = T_STRING;
                        }
                        else
                        {
                            // TODO
                        }
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
                    call_func(var_glob, local_table, global_table);
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
            gen_value(output, &current_token, false, NULL); // TODO: overit
        }
    }
    
    if (nest_level == 0)
    {
        gen_assign(output, &identifier);
    }
    else
    {
        gen_assign(output, &identifier);
    }
    //current_token = getTokenAssertArr(2, (token_type_t[]){TOK_EOF, TOK_EOL});
    current_token = getToken();
    if (current_token.type == TOK_EOF)
    {
        token_table.index--;
    }
    
    var_type.type = kw_to_token_type(var_type.type);
    
    
    local_table->table = local_insert(local_table->table, &identifier.attribute.str, var_type.type, var_type.attribute.includesNil, is_constant, true);
}

void handle_assign_or_call_func(token_t token_id, global_symtab_t *global_table, local_symtab_w_par_ptr_t *local_table, int nest_level)
{
    token_t current_token = getToken();
    if (current_token.type == TOK_L_BRCKT) // za id ihned '('
    {
        ungetToken();
        global_symtab_t* func = global_search(global_table, &token_id.attribute.str);
        if (func == NULL || func->is_func == false) // pokud neni funkce
        {
            // error - nedefinovana funkce
            returnError(FUNCTION_DEFINITION_ERR);
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
            ((global_symtab_t*)var)->isInitialised = true;
        }
        else
        {
            var_type = ((local_symtab_t*)var)->type;
            if (((local_symtab_t*)var)->isConstant == true)
            {
                // error - prirazeni do konstanty
                returnError(OTHER_ERR);
            }
            ((local_symtab_t*)var)->isInitialised = true;
        }

        current_token = getTokenAssertArr(5, (token_type_t[]){TOK_KW_NIL, TOK_INT, TOK_DOUBLE, TOK_STRING, TOK_IDENTIFIER});

        current_token = getToken();


        ungetToken();
        ungetToken();

        // TODO: zmenit poradi

        if (current_token.type != TOK_EOF && current_token.type != TOK_EOL)
        {
            token_type_t type_expr = checkExpression(local_table, global_table);
            if (var_type != type_expr)
            {
                // error - spatny typ
                returnError(TYPE_COMPATIBILITY_ERR);
            }
        }
        else
        {
            current_token = getToken();
            if (current_token.type != TOK_IDENTIFIER)
            {
                if (current_token.type != var_type)
                {
                    // error - spatny typ
                    returnError(TYPE_COMPATIBILITY_ERR);
                }
                gen_value(output, &current_token, false, NULL);   
            }
            else if (global_search(global_table, &current_token.attribute.str) != NULL)
            {
                if(global_search(global_table, &current_token.attribute.str)->is_func == true)
                {
                    call_func(global_search(global_table, &current_token.attribute.str), local_table, global_table);
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
        



        if (nest_level == 0)
        {
            gen_assign(output, &token_id);
        }
        else
        {
            gen_assign(output, &token_id);
        }
    }
    else
    {
        // error
        returnError(SYNTAX_ERR);
    }
}

void handle_func_def(global_symtab_t *global_table, local_symtab_w_par_ptr_t *local_table_one_up)
{
    token_t token = getTokenAssert(TOK_IDENTIFIER);
    global_symtab_t *found  = global_search(global_table, &token.attribute.str);
    if (found == NULL)
    {
        // error - nevim jak by se stalo, mozna neni potreba
        returnError(FUNCTION_DEFINITION_ERR);
    }
    if (found->is_func == false) // dont bugfix at 2am :skull:
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
    getTokenAssert(TOK_L_BRCKT);
    while (getToken().type != TOK_R_BRCKT)
    {
        ;
    }
    if (getToken().type == TOK_ARROW)
    {
        getTokenAssertArr(3, (token_type_t[]){TOK_KW_DOUBLE, TOK_KW_INT, TOK_KW_STRING});
    }
    else
    {
        ungetToken();
    }
    token_t current_token = getTokenAssertArr(2, (token_type_t[]){TOK_L_CRL_BRCKT, TOK_EOL});
    if (current_token.type == TOK_EOL)
    {
        current_token = getTokenAssert(TOK_L_CRL_BRCKT);
    }
    
    local_symtab_w_par_ptr_t local_table;
    local_init_w_par_ptr_t(&local_table);
    local_table.parent = local_table_one_up;

    for (int i = 0; i < found->param_count; i++)
    {
        local_table.table = local_insert(local_table.table, &found->params[i].identifier, found->params[i].type, found->params[i].includesNil, true, true); // paramentry nelze menit uvnitr funkce, asi?
    }

    if (strcmp(found->key.data, "write")        == 0
    ||  strcmp(found->key.data, "readString")   == 0
    ||  strcmp(found->key.data, "readInt")      == 0
    ||  strcmp(found->key.data, "readDouble")   == 0
    ||  strcmp(found->key.data, "Int2Double")   == 0
    ||  strcmp(found->key.data, "Double2Int")   == 0
    ||  strcmp(found->key.data, "length")       == 0
    ||  strcmp(found->key.data, "ord")          == 0
    ||  strcmp(found->key.data, "chr")          == 0)
    {
        // toto nepassovat do generatoru - built-in funkce
        getTokenAssert(TOK_R_CRL_BRCKT);
    }
    else
    {
        parse_block(-1000, TOK_L_CRL_BRCKT, global_table, &local_table);
        gen_func_end(output, &token);
    }
}

void handle_cond(local_symtab_w_par_ptr_t *local_table, global_symtab_t *global_table)
{
    //token_t current_token = getToken();
    checkExpression(local_table, global_table);
}

bool handle_if(int nest_level, local_symtab_w_par_ptr_t *local_table, global_symtab_t *global_table)
{
    bool has_return = true;
    token_t current_token = getToken();

    if (current_token.type == TOK_EOL)
    {
        current_token = getToken();
    }
    

    if (current_token.type == TOK_L_BRCKT)
    {
        handle_cond(local_table, global_table);
        gen_if(output, counter);
        getTokenAssert(TOK_R_BRCKT);
        if (getToken().type != TOK_EOL)
        {
            ungetToken();
        }
        getTokenAssert(TOK_L_CRL_BRCKT);
        if(parse_block(nest_level + 1, TOK_L_CRL_BRCKT, global_table, local_table) == false)
        {
            has_return = false;
        }
    }
    else if (current_token.type == TOK_KW_LET)
    {
        current_token = getTokenAssert(TOK_IDENTIFIER);
        void* var = local_search_in_all(local_table, &current_token.attribute.str);
        if(var == NULL)
        {
            var = global_search(global_table, &current_token.attribute.str);
            if (var == NULL)
            {
                // error - nedefinovana promenna
                returnError(VARIABLE_DEFINITION_ERR);
            }
            if (((global_symtab_t*)var)->includesNil == false)
            {
                // error - non-nil promenna v if-let
                returnError(OTHER_ERR);
            }
            if (parse_block(nest_level + 1, TOK_L_CRL_BRCKT, global_table, local_table) == false)
            {
                has_return = false;
            }
        }
        else
        {
            if (((local_symtab_t*)var)->includesNil == false)
            {
                // error - non-nil promenna v if-let
                returnError(OTHER_ERR);
            }
            if (parse_block(nest_level + 1, TOK_L_CRL_BRCKT, global_table, local_table) == false)
            {
                has_return = false;
            }
        }
    }
    else
    {
        ungetToken();
        handle_cond(local_table, global_table);
        gen_if(output, counter);
        getTokenAssert(TOK_L_CRL_BRCKT);
        if(parse_block(nest_level + 1, TOK_L_CRL_BRCKT, global_table, local_table) == false)
        {
            has_return = false;
        }
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
        getTokenAssert(TOK_L_CRL_BRCKT);
        gen_else(output, counter);
        if (parse_block(nest_level + 1, TOK_L_CRL_BRCKT, global_table, local_table) == false)
        {
            has_return = false;
        }
    }
    else
    {
        ungetToken();
    }
    gen_if_end(output, counter);
    return has_return;
}

void handle_while(int nest_level, local_symtab_w_par_ptr_t *local_table, global_symtab_t *global_table)
{
    getTokenAssert(TOK_L_BRCKT);
    gen_while(output, counter);
    handle_cond(local_table, global_table);
    gen_while_body(output, counter);
    getTokenAssert(TOK_R_BRCKT);
    getTokenAssert(TOK_L_CRL_BRCKT);
    parse_block(nest_level + 1, TOK_L_CRL_BRCKT, global_table, local_table);
    gen_while_end(output, counter);
    counter++;
}

bool parse_block(int nest_level, token_type_t block_start, global_symtab_t *global_table, local_symtab_w_par_ptr_t *local_table_one_up) // returnuje posledni token
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
        // error
        returnError(SYNTAX_ERR);
    }

    token_t current_token;
    current_token = getToken();

    local_symtab_w_par_ptr_t local_table;
    local_init_w_par_ptr_t(&local_table);
    local_table.parent = local_table_one_up; // checknout jeste


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
            getTokenAssert(TOK_BLOCK_COM_END);
        }
        else if (current_token.type == TOK_KW_RETURN)
        {
            // TODO returns, read expr
            while (current_token.type != TOK_EOL && current_token.type != TOK_EOF)
            {
                current_token = getToken();
            }
        }
        else if (current_token.type == TOK_L_CRL_BRCKT)
        {
            parse_block(nest_level + 1, TOK_L_CRL_BRCKT, global_table, &local_table);
        }
        else if (current_token.type == TOK_R_CRL_BRCKT)
        {
            // error
            returnError(SYNTAX_ERR);
        }
        else if (current_token.type == TOK_KW_LET || current_token.type == TOK_KW_VAR)
        {
            handle_variable(current_token, global_table, &local_table, nest_level);
        }
        else if (current_token.type == TOK_IDENTIFIER)
        {
            handle_assign_or_call_func(current_token, global_table, &local_table, nest_level);
        }
        else if (current_token.type == TOK_KW_FUNC)
        {
            if (nest_level != 0)
            {
                // error - funkce definovana v podbloku
                returnError(FUNCTION_DEFINITION_ERR);
            }
            else
            {
                handle_func_def(global_table, &local_table);
            }
        }
        else if (current_token.type == TOK_KW_IF)
        {
            handle_if(nest_level, &local_table, global_table);
        }
        else if (current_token.type == TOK_KW_WHILE)
        {
            handle_while(nest_level, &local_table, global_table);
        }
        else if (current_token.type == TOK_KW_RETURN)
        {
            has_return = true;
            if (nest_level == 0)
            {
                // error - return v mainu, asi error
                returnError(SYNTAX_ERR);
            }
            if (nest_level == -MAX_NEST_LEVEL) // telo funkce
            {
                // TODO
            }
        }
        current_token = getToken();
    }
    return has_return;
}

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
            getTokenAssert(TOK_BLOCK_COM_END);
        } 
        else if (current_token.type == TOK_L_CRL_BRCKT || current_token.type == TOK_L_BRCKT)
        {
            read_subblock(current_token);
        }
        else if (current_token.type == TOK_R_CRL_BRCKT || current_token.type == TOK_R_BRCKT) // (...} nebo {...)
        {
            // error
            returnError(SYNTAX_ERR);
        }
        else if (current_token.type == TOK_KW_FUNC)
        {
            // error - funkce definovana v podbloku
            returnError(SYNTAX_ERR);
        }
        current_token = getToken();
    }
    return;
}

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
            getTokenAssert(TOK_BLOCK_COM_END);
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
            token_t func_name = getTokenAssert(TOK_IDENTIFIER);

            global_symtab_t* func = global_search(*global_table, &func_name.attribute.str);
            if (func != NULL)
            {
                // error redefinice
                returnError(FUNCTION_DEFINITION_ERR);
            }
            current_token = getTokenAssert(TOK_L_BRCKT);
            func_param_t *params;
            int param_cntr = 0;
            current_token = getToken();
            while (current_token.type != TOK_R_BRCKT)
            {
                token_t name = current_token;
                if (name.type == TOK_UNDERSCORE)
                {
                    //dstringAppend(&name.attribute.str, '_'); 
                }
                else if (name.type != TOK_IDENTIFIER)
                {
                    // error
                    returnError(SYNTAX_ERR);
                }
                
                token_t id = getTokenAssert(TOK_IDENTIFIER);

                if (dstringCompare(&name.attribute.str, &id.attribute.str) == 0)
                {
                    // error - jmeno a id se nesmi rovnat
                    returnError(OTHER_ERR); // asi 9?
                }

                getTokenAssert(TOK_COLON);

                current_token = getTokenAssertArr(3, (token_type_t[]){TOK_KW_DOUBLE, TOK_KW_INT, TOK_KW_STRING});

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
                    
                    func_param_t* tmp = realloc(params, sizeof(func_param_t) * param_cntr + 1); // neni idealni, ale asi nebude tolik paramentru aby na tom zalezelo
                    if (tmp == 0)
                    {
                        // error
                        returnError(INTERN_ERR);
                    }
                    params = tmp;
                }

                params[param_cntr].name = name.attribute.str;
                params[param_cntr].identifier = id.attribute.str;
                params[param_cntr].type = current_token.type;
                params[param_cntr].includesNil = current_token.attribute.includesNil;

                current_token = getTokenAssertArr(2, (token_type_t[]){TOK_COMMA, TOK_R_BRCKT});

                if (current_token.type == TOK_COMMA)
                {
                    current_token = getToken();
                }

                param_cntr++;
            }

            if (param_cntr == 0) // TODO: toto je asi zbytecne, jelikoz is_func existuje
            {
                params = malloc(sizeof(func_param_t));
            }

            current_token = getToken();
            if (current_token.type == TOK_ARROW)
            {
                getTokenAssertArr(3, (token_type_t[]){TOK_KW_DOUBLE, TOK_KW_INT, TOK_KW_STRING}); // TODO: toto crashne pokud má funkce >1 parametr
                type_t current_token_type;
                if (current_token.type == TOK_INT)
                {
                    current_token_type = T_INT;
                }
                else if (current_token.type == TOK_DOUBLE)
                {
                    current_token_type = T_DOUBLE;
                }
                else if (current_token.type == TOK_STRING)
                {
                    current_token_type = T_STRING;
                }
                *global_table = global_insert(*global_table, &func_name.attribute.str, current_token_type, true, param_cntr, params, current_token.attribute.includesNil, true, true);
            }
            else
            {
                *global_table = global_insert(*global_table, &func_name.attribute.str, T_NIL, true, param_cntr, params, current_token.attribute.includesNil, true, true); // void funkce
                continue;
            }
            
            // obsah uvnitr funkce resim v 2. pruchodu
        }
        current_token = getToken();
    }
    current_token.type = TOK_EOF;
    addTableToken(&token_table, current_token);
}

int parse()
{
    global_symtab_t *global_table;
    global_init(&global_table);
    load_built_in_functions();
    initTokenTable(&token_table);
    find_functions(&global_table);
    token_table.insert = false;
    token_table.index = 0;
    parse_block(0, TOK_NOTHING, global_table, NULL);
    freeTokenTable(&token_table);
    global_dispose(&global_table);
    return 1;
}