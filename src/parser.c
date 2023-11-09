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
    return table->tokens[table->index];
    table->index++;
}

token_t getTokenAssert(token_type_t type) // getToken, vraci chybu pokud obdrzi jiny token nez ocekavany
{
    token_t token = getToken();
    if (token.type != type)
    {
        // error
    }
    return token;
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

int handle_assignment(token_t token_assigner, global_symtab_t *global_table, local_symtab_w_par_ptr_t *local_table)
{
    token_t identifier;
    token_t var_type;
    bool is_constant = false;

    if (token_assigner.type == TOK_KW_LET) // let uvádí konstantní proměnnou
    {
        is_constant = true;
    }

    token_t current_token = getTokenAssert(TOK_IDENTIFIER);
    identifier = current_token;

    current_token = getToken();

    if (current_token.type == TOK_COLON)
    {
        current_token = getToken();
        var_type = current_token;
        if (var_type.type != TOK_KW_DOUBLE && var_type.type != TOK_KW_INT && var_type.type != TOK_KW_STRING)
        {
            //error
        }
        current_token = getTokenAssert(TOK_EQUAL);
        // bottom-up parsing
    }
    else {
        if (current_token.type != TOK_EQUAL)
        {
            //error
        }

       if (current_token.type == TOK_SEMICLN) // nebo newline
       {
           //local_insert()
       }
    }

}

int parse_block(int nest_level, global_symtab_t *global_table, local_symtab_w_par_ptr_t *local_table_one_up)
{
    token_t current_token;
    current_token = getToken();

    local_symtab_w_par_ptr_t local_table;
    local_init_w_par_ptr_t(&local_table);

    if (nest_level == 0) // main block
    {
        ;
    }

    while (current_token.type != TOK_EOF) {
        if (current_token.type == TOK_COMMENT)
        {
            ;
        }
        if (current_token.type == TOK_BLOCK_COM_START)
        {
            int comment_nest_level = 1;
            while (comment_nest_level != 0)
            {
                current_token = getToken();
                if (current_token.type == TOK_BLOCK_COM_START)
                {
                    comment_nest_level++;
                }
                else if (current_token.type == TOK_BLOCK_COM_END)
                {
                    comment_nest_level--;
                }
            }
            continue;
        }
        else if (current_token.type = TOK_L_CRL_BRCKT)
        {
            parse_block(nest_level + 1, global_table, &local_table);
        }
        else if (current_token.type = TOK_R_CRL_BRCKT)
        {
            // error
        }
        else if (current_token.type == TOK_KW_LET || current_token.type == TOK_KW_VAR)
        {
            handle_assignment(current_token, global_table, &local_table);
            
        }
        current_token = getToken();
    }
}

void read_subblock(token_t token)
{
    token_t block_end;
    if (token.type == '(')
    {
        block_end.type = ')';
    }
    else if (token.type == '{')
    {
        block_end.type = '}';
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
            int comment_nest_level = 1;
            while (comment_nest_level != 0)
            {
                current_token = getToken();
                if (current_token.type == TOK_BLOCK_COM_START)
                {
                    comment_nest_level++;
                }
                else if (current_token.type == TOK_BLOCK_COM_END)
                {
                    comment_nest_level--;
                }
            }
        } 
        else if (current_token.type == TOK_L_CRL_BRCKT || current_token.type == TOK_L_BRCKT)
        {
            read_subblock(current_token);
        }
        else if (current_token.type == TOK_R_CRL_BRCKT || current_token.type == TOK_R_BRCKT)
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

void init_func_table(func_table_t *table)
{
    table->capacity = 64;
    table->size = 0;
    table->members = (func_table_member_t *)malloc(sizeof(func_table_member_t) * table->capacity);
    if (table->members == NULL)
    {
        // malloc error
    }
}

void add_to_func_table(func_table_t *table, func_table_member_t *member)
{
    if (table->size >= table->capacity)
    {
        resize_func_table(table);
    }
    table->members[table->size] = *member;
    table->size++;
}

void resize_func_table(func_table_t *table)
{
    table->capacity *= 2;
    table->members = (func_table_member_t *)realloc(table->members, sizeof(func_table_member_t) * table->capacity);
    if (table->members == NULL)
    {
        // realloc error
    }
}

int find_functions()
{
    func_table_t func_table;
    init_func_table(&func_table);
    token_t current_token;
    current_token = getToken();

    while (current_token.type != TOK_EOF) {
        if (current_token.type == TOK_COMMENT)
        {
            ;
        }
        else if (current_token.type == TOK_BLOCK_COM_START)
        {
            int comment_nest_level = 1;
            while (comment_nest_level != 0)
            {
                current_token = getToken();
                if (current_token.type == TOK_BLOCK_COM_START)
                {
                    comment_nest_level++;
                }
                else if (current_token.type == TOK_BLOCK_COM_END)
                {
                    comment_nest_level--;
                }
            }
            continue;
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
            func_table_member_t func_table_member;
            dstringInit(&(func_table_member.key));
            current_token = getTokenAssert(TOK_IDENTIFIER);
            // TODO funkce nesmi byt zabudovana/jiz definovana
            dstringCopy(&func_table_member.key, &current_token.attribute.str);
            current_token = getToken();
            getTokenAssert(TOK_L_BRCKT);
            int param_cntr = 0;
            while (current_token.type != TOK_R_BRCKT)
            {
                dstringInit(&func_table_member.params[param_cntr].name.str);
                dstringInit(&func_table_member.params[param_cntr].identifier.str);
                current_token = getToken();
                if (current_token.type == TOK_KW_NIL)
                {
                    dstringAppend(&func_table_member.params[param_cntr].name.str, '_');
                }
                else if (current_token.type == TOK_IDENTIFIER)
                {
                    dstringCopy(&func_table_member.params[param_cntr].name.str, &current_token.attribute.str);
                }
                else
                {
                    // error
                }
                

                current_token = getTokenAssert(TOK_IDENTIFIER);
                dstringCopy(&func_table_member.params[param_cntr].identifier.str, &current_token.attribute.str);

                current_token = getTokenAssert(TOK_COLON);

                current_token = getToken();
                if (current_token.type != TOK_KW_DOUBLE && current_token.type != TOK_KW_INT && current_token.type != TOK_KW_STRING)
                {
                    // error
                }
                func_table_member.params[param_cntr].type = current_token.type;

                current_token = getTokenAssert(TOK_COMMA);

                param_cntr++;
            }
            current_token = getTokenAssert(TOK_ARROW);

            current_token = getToken();
            if (current_token.type == TOK_KW_DOUBLE || current_token.type == TOK_KW_INT || current_token.type == TOK_KW_STRING)
            {
                func_table_member.type = current_token.type;
            }
            else
            {
                // error
            }
            // obsah uvnitr funkce resim v 2. pruchodu
            add_to_func_table(&func_table, &func_table_member);
        }
        current_token = getToken();
    }
}

int parse()
{
    initTokenTable(&token_table);
    find_functions();
    token_table.insert == false;
    global_symtab_t *global_table;
    global_init(&global_table);
    //parse_block(0, global_table, NULL);
    freeTokenTable(&token_table);
    global_dispose(&global_table);
    return 1;
}