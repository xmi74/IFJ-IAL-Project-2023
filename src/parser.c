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

#define MAX_NEST_LEVEL 1000

token_table_t token_table;

void initTokenTable(token_table_t *table)
{
    table->insert = true;
    table->capacity = 64;
    table->size = 0;
    table->index = 0;
    table->tokens = malloc(table->capacity * sizeof(token_t));
    if (table->tokens == NULL)
    {
        // error
    }    
}

void freeTokenTable(token_table_t *table)
{
    free(table->tokens);
}

void addTableToken(token_table_t *table, token_t token)
{
    if (table->capacity<=table->size)
    {
        table->capacity *= 2;
        table->tokens = realloc(table->tokens, table->capacity * sizeof(token_t)); // asi fixnout memleak
        if (table->tokens == NULL)
        {
            // error
        }
    }
    table->tokens[table->size] = token;
    table->size++;
}

token_t getTableToken(token_table_t *table)
{
    if(table->size > table->capacity)
    {
        token_t eof_token;
        eof_token.type = TOK_EOF;
        return eof_token;
    }
    return table->tokens[table->index++];
}

token_t getTokenAssert(token_type_t type) // getToken, vraci chybu pokud obdrzi jiny token nez ocekavany
{
    token_t token = getToken();
    if (token.type != type)
    {
        // error
        returnError(SYNTAX_ERR); // TODO, placeholder
    }
    return token;
}

token_t getTokenAssertArr(token_type_t *type) // getToken, vraci chybu pokud obdrzi jiny token nez ze seznamu ocekavanych
{
    token_t token = getToken();
    for(int i = 0; i < sizeof(type)/sizeof(type[0]); i++)
    {
        if (token.type == type[i])
        {
            return token;
        }
    }
    // error
    returnError(SYNTAX_ERR); // TODO, placeholder
}

token_t getToken()
{
    if(token_table.insert == true)
    {
        token_t token = getNextToken();
        addTableToken(&token_table, token);
        return token;
    }
    else
    {
        return getTableToken(&token_table);
    }
}

int call_func(global_symtab_t *func, local_symtab_w_par_ptr_t *local_table, global_symtab_t *global_table)
{
    token_t current_token;
    for (int i = 0; i < global_table->param_count; i++)
    {
        if(func->params[i].type != TOK_NOTHING)
        {
            current_token = getTokenAssert(TOK_IDENTIFIER);
            dstringCompare(&current_token.attribute.str, &func->params[i].name);
        }
        if(func->params[i].type == TOK_NOTHING) // _
        {
            current_token = getToken();
            if (current_token.type == TOK_IDENTIFIER)
            {
                // TODO najit v symtable
                // TODO: AST - pridat
            }
            else if (current_token.type == func->params[i].type) // literal
            {
                // TODO: AST - pridat
            }
        }
    }
}

int handle_variable(token_t token_assigner, global_symtab_t *global_table, local_symtab_w_par_ptr_t *local_table)
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

    token_t current_token = getTokenAssertArr((token_type_t[]){TOK_EQUAL, TOK_COLON});

    if (current_token.type == TOK_COLON)
    {
        var_type = getTokenAssertArr((token_type_t[]){TOK_KW_DOUBLE, TOK_KW_INT, TOK_KW_STRING});
        getTokenAssert(TOK_EQUAL);
    }

    current_token = getToken();

    global_symtab_t* func = global_search(global_table, &current_token.attribute.str);

    if (func == NULL || func->is_func == false) // pokud neni funkce
    {   
        // TODO - bottom-up
    }
    else
    {
       // TODO - call func 
    }

    current_token = getTokenAssertArr((token_type_t[]){TOK_EOF, TOK_EOL});
    if (current_token.type == TOK_EOF)
    {
        token_table.index--;
    }

    local_insert(local_table->table, &identifier.attribute.str, identifier.type);
}

int handle_assign_or_call_func(token_t token_id, global_symtab_t *global_table, local_symtab_w_par_ptr_t *local_table)
{
    token_t current_token = getToken();
    if (current_token.type == TOK_L_BRCKT) // za id ihned '('
    {
        global_symtab_t *func = global_search(global_table, &token_id.attribute.str);
        if (func == NULL)
        {
            // error - funkce neexistuje
        }
        for (int i = 0; i < global_table->param_count; i++)
        {
            if(func->params[i].type != TOK_NOTHING)
            {
                current_token = getTokenAssert(TOK_IDENTIFIER);
                dstringCompare(&current_token.attribute.str, &func->params[i].name);
            }
            if(func->params[i].type == TOK_NOTHING) // _
            {
                current_token = getToken();
                if (current_token.type == TOK_IDENTIFIER)
                {
                    // TODO najit v symtable
                    // TODO: AST - pridat
                }
                else if (current_token.type == func->params[i].type) // literal
                {
                    // TODO: AST - pridat
                }
            }
        }
        
        getTokenAssert(TOK_R_BRCKT);
        // TODO: AST - zavolat funkci
    }
    else if (current_token.type == TOK_EQUAL)
    {
        // assign
    }
    else
    {
        // error
    }
}

int handle_func_def(token_t token, global_symtab_t *global_table, local_symtab_w_par_ptr_t *local_table)
{
    void *found = local_search_in_all(local_table, &token.attribute.str);
    if (found == NULL)
    {
        found = global_search(global_table, &token.attribute.str); // TODO ASAP
        if (found == NULL)
        {
            // error - nevim jak by se stalo, mozna neni potreba
        }
    }
    // spravnost definice se uz kontroluje v 1. pruchodu
    getTokenAssert(TOK_IDENTIFIER);
    getTokenAssert(TOK_L_BRCKT);
    while (getToken().type != TOK_R_BRCKT)
    {
        ;
    }
    if (getToken().type == TOK_ARROW)
    {
        getTokenAssertArr((token_type_t[]){TOK_KW_DOUBLE, TOK_KW_INT, TOK_KW_STRING});
    }
    token_t current_token = getTokenAssertArr((token_type_t[]){TOK_L_CRL_BRCKT, TOK_EOL});
    if (current_token.type == TOK_EOL)
    {
        current_token = getTokenAssert(TOK_L_CRL_BRCKT);
    }

    parse_block(-1000, global_table, local_table);
    
}
token_t parse_block(int nest_level, global_symtab_t *global_table, local_symtab_w_par_ptr_t *local_table_one_up) // returnuje posledni token
{
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
    }

    while (token_table.index < token_table.size) { //  && current_token.type != TOK_R_BRCKT && current_token.type != TOK_R_CRL_BRCKT
        if (current_token.type == TOK_COMMENT)
        {
            ;
        }
        else if (current_token.type == TOK_BLOCK_COM_START)
        {
            getTokenAssert(TOK_BLOCK_COM_END);
        }
        else if (current_token.type == TOK_L_CRL_BRCKT)
        {
            parse_block(nest_level + 1, global_table, &local_table);
        }
        else if (current_token.type == TOK_R_CRL_BRCKT)
        {
            // error
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
            }
            else
            {
                handle_func_def(current_token, global_table, &local_table);
            }
        }
        else if (current_token.type == TOK_KW_IF)
        {
            // TODO

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
        else if (current_token.type == TOK_KW_WHILE)
        {
            // TODO
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
    }

    token_t current_token = getToken();
    while (current_token.type != block_end.type)
    {
        if (current_token.type == TOK_EOF)
        {
            // error
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
        }
        else if (current_token.type == TOK_KW_FUNC)
        {
            // error - funkce definovana v podbloku
        }
        current_token = getToken();
    }
    return;
}

void find_functions(global_symtab_t **global_table)
{
    token_t current_token;
    current_token = getToken();

    while (current_token.type != TOK_EOF && current_token.type != TOK_COMMENT) { // TEST
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
        }
        else if (current_token.type == TOK_KW_FUNC) // neresim kde je definovana, jen ze ne v podbloku
        {
            token_t func_name = getTokenAssert(TOK_IDENTIFIER);
            // TODO funkce nesmi byt zabudovana/jiz definovana
            current_token = getTokenAssert(TOK_L_BRCKT);
            func_param_t *params;
            int param_cntr = 0;
            current_token = getToken();
            while (current_token.type != TOK_R_BRCKT)
            {
                token_t name = current_token;
                if (name.type == TOK_KW_NIL)
                {
                    //dstringAppend(&name.attrbute.str, '_'); 
                }
                else if (name.type != TOK_IDENTIFIER)
                {
                    // error
                }
                
                token_t id = getTokenAssert(TOK_IDENTIFIER);

                if (dstringCompare(&name.attribute.str, &id.attribute.str) == 0)
                {
                    // error - jmeno a id se nesmi rovnat
                }

                getTokenAssert(TOK_COLON);

                current_token = getTokenAssertArr((token_type_t[]){TOK_KW_DOUBLE, TOK_KW_INT, TOK_KW_STRING});

                if (param_cntr == 0)
                {
                    params = malloc(sizeof(func_param_t));
                    if (params == 0)
                    {
                        // error - malloc fail
                    }
                }
                else
                {
                    params = realloc(params, sizeof(func_param_t) * param_cntr + 1); // neni idealni, ale asi nebude tolik paramentru aby na tom zalezelo
                    if (params == 0)
                    {
                        // error
                    }
                }

                params[param_cntr].name = name.attribute.str;
                params[param_cntr].identifier = id.attribute.str;
                params[param_cntr].type = current_token.type;

                current_token = getTokenAssert(TOK_COMMA);

                param_cntr++;
                current_token = getToken();
            }

            if (param_cntr == 0)
            {
                params = malloc(sizeof(func_param_t));
            }

            current_token = getToken();
            if (current_token.type == TOK_ARROW)
            {
                getTokenAssertArr((token_type_t[]){TOK_KW_DOUBLE, TOK_KW_INT, TOK_KW_STRING});
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
    initTokenTable(&token_table);
    find_functions(&global_table);
    token_table.insert = false;
    token_table.index = 0;
    parse_block(0, global_table, NULL);
    freeTokenTable(&token_table);
    global_dispose(&global_table);
    return 1;
}