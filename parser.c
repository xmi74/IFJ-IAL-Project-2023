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

void load_built_in_functions(global_symtab_t **global_table)
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
    if (strcmp(func->key.data, "write"))
    {
        // TODO: write ma neomezeno parametru
    }
    token_t current_token;
    getTokenAssert(TOK_L_BRCKT);
    for (int i = 0; i < func->param_count; i++)
    {
        if (strcmp(func->params[i].name.data, "_") != 0)
        {
            current_token = getTokenAssert(TOK_IDENTIFIER);
            if (dstringCompare(&current_token.attribute.str, &func->params[i].name) != 0)
            {
                returnError(FUNCTION_USAGE_ERR); // TODO: zkontrolovat, ze FUNCTION_USAGE_ERR
                // error - spatne jmeno parametru
            }
            getTokenAssert(TOK_COLON);
        }

        current_token = getTokenAssertArr(5, (token_type_t[]){TOK_INT, TOK_DOUBLE, TOK_STRING, TOK_KW_NIL, TOK_IDENTIFIER});
        if (current_token.type == TOK_IDENTIFIER)
        {
            void* var = local_search_in_all(local_table, &current_token.attribute.str);
            if (var == NULL)
            {
                var = global_search(global_table, &current_token.attribute.str);
                if (var == NULL)
                {
                    returnError(VARIABLE_DEFINITION_ERR);
                    // error - nedefinovana promenna
                }
            }
        }
        if (i < global_table->param_count - 1)
        {
            getTokenAssert(TOK_COMMA);
        }
        gen_value(output, &current_token);
    }
    getTokenAssert(TOK_R_BRCKT);
    gen_func_call(output, func->key.data);
}

void handle_variable(token_t token_assigner, global_symtab_t *global_table, local_symtab_w_par_ptr_t *local_table)
{
    token_t var_type;
    bool is_constant = false;

    if (token_assigner.type == TOK_KW_LET) // let uvádí konstantní proměnnou
    {
        is_constant = true;
    }

    token_t identifier = getTokenAssert(TOK_IDENTIFIER);

    if(local_search(local_table->table, &identifier.attribute.str) != NULL)
    {
        // error - redefinice promenne
        returnError(VARIABLE_DEFINITION_ERR);
    }

    token_t current_token = getTokenAssertArr(2, (token_type_t[]){TOK_ASSIGN, TOK_COLON});

    if (current_token.type == TOK_COLON)
    {
        var_type = getTokenAssertArr(3, (token_type_t[]){TOK_KW_DOUBLE, TOK_KW_INT, TOK_KW_STRING});
        getTokenAssert(TOK_ASSIGN);
    }
    gen_var(output, &identifier, true);
    current_token = getToken();

    //global_symtab_t* func = global_search(global_table, &current_token.attribute.str);
    global_symtab_t* func = global_search(global_table, &identifier.attribute.str);

    if (func == NULL || func->is_func == false) // pokud neni funkce
    {   
        ungetToken();
        checkExpression(local_table, global_table);
    }
    else
    {
        call_func(func, local_table, global_table);
    }
    gen_assign(output, &identifier, true);
    //current_token = getTokenAssertArr(2, (token_type_t[]){TOK_EOF, TOK_EOL});
    current_token = getToken();
    if (current_token.type == TOK_EOF)
    {
        token_table.index--;
    }

    local_insert(local_table->table, &identifier.attribute.str, identifier.type);
}

void handle_assign_or_call_func(token_t token_id, global_symtab_t *global_table, local_symtab_w_par_ptr_t *local_table)
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
        // TODO: codegen
    }
    else if (current_token.type == TOK_ASSIGN)
    {
        void* var = local_search_in_all(local_table, &token_id.attribute.str);
        if (var == NULL)
        {
            var = global_search(global_table, &token_id.attribute.str);
            if (var == NULL)
            {
                // error - nedefinovana promenna
                returnError(VARIABLE_DEFINITION_ERR);
            }
        }
        current_token = getToken();
        if (global_search(global_table, &current_token.attribute.str) != NULL)
        {
            call_func(global_search(global_table, &current_token.attribute.str), local_table, global_table);
        }
        else
        {
            ungetToken();
            checkExpression(local_table, global_table);
        }
        gen_assign(output, &token_id, true);
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
    gen_func(output, &token);
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
        local_table.table = local_insert(local_table.table, &found->params[i].identifier, found->params[i].type);
    }

    parse_block(-1000, TOK_L_CRL_BRCKT, global_table, &local_table);
    gen_func_end(output, &token);
}

void handle_cond(local_symtab_w_par_ptr_t *local_table, global_symtab_t *global_table)
{
    token_t current_token = getToken();
    checkExpression(local_table, global_table);
}

void handle_if(int nest_level, local_symtab_w_par_ptr_t *local_table, global_symtab_t *global_table)
{
    token_t current_token = getToken();

    if (current_token.type == TOK_L_BRCKT)
    {
        handle_cond(local_table, global_table);
        gen_if(output, counter);
        getTokenAssert(TOK_R_BRCKT);
        getTokenAssert(TOK_L_CRL_BRCKT);
        parse_block(nest_level + 1, TOK_L_CRL_BRCKT, global_table, local_table);
    }
    else if (current_token.type == TOK_KW_LET)
    {
        current_token = getTokenAssert(TOK_IDENTIFIER);
        void* var = local_search_in_all(local_table, &current_token.attribute.str);
        if(var = NULL)
        {
            var = global_search(global_table, &current_token.attribute.str);
            if (var == NULL)
            {
                // error - nedefinovana promenna
                returnError(VARIABLE_DEFINITION_ERR);
            }
        }
    }
    

    current_token = getToken();

    if(current_token.type = TOK_EOL)
    {
        current_token = getToken();
    }

    if (current_token.type == TOK_KW_ELSE)
    {
        //TODO
    }
}

void handle_while(int nest_level, local_symtab_w_par_ptr_t *local_table, global_symtab_t *global_table)
{
    token_t current_token = getTokenAssert(TOK_L_BRCKT);
    gen_while(output, counter);
    handle_cond(local_table, global_table);
    gen_while_body(output, counter);
    getTokenAssert(TOK_R_BRCKT);
    getTokenAssert(TOK_L_CRL_BRCKT);
    parse_block(nest_level + 1, TOK_L_CRL_BRCKT, global_table, local_table);
    gen_while_end(output, counter);
    counter++;
}

token_t parse_block(int nest_level, token_type_t block_start, global_symtab_t *global_table, local_symtab_w_par_ptr_t *local_table_one_up) // returnuje posledni token
{
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
            handle_variable(current_token, global_table, &local_table);
        }
        else if (current_token.type == TOK_IDENTIFIER)
        {
            handle_assign_or_call_func(current_token, global_table, &local_table);
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
        current_token = getToken();
    }
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
            // TODO funkce nesmi byt zabudovana

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

                current_token = getTokenAssertArr(2, (token_type_t[]){TOK_COMMA, TOK_R_BRCKT});

                if (current_token.type == TOK_COMMA)
                {
                    current_token = getToken();
                }

                param_cntr++;
            }

            if (param_cntr == 0)
            {
                params = malloc(sizeof(func_param_t));
            }

            current_token = getToken();
            if (current_token.type == TOK_ARROW)
            {
                getTokenAssertArr(3, (token_type_t[]){TOK_KW_DOUBLE, TOK_KW_INT, TOK_KW_STRING}); // TODO: toto crashne pokud má funkce >1 parametr
                *global_table = global_insert(*global_table, &func_name.attribute.str, current_token.type, true, param_cntr, params);
            }
            else
            {
                *global_table = global_insert(*global_table, &func_name.attribute.str, TOK_NOTHING, true, param_cntr, params); // void funkce
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
    load_built_in_functions(&global_table);
    initTokenTable(&token_table);
    find_functions(&global_table);
    token_table.insert = false;
    token_table.index = 0;
    parse_block(0, TOK_NOTHING, global_table, NULL);
    freeTokenTable(&token_table);
    global_dispose(&global_table);
    return 1;
}