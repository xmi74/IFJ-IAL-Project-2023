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


//keyword_t keyword_type(token_t token) // rozliseni identifikatoru a klicovych slov
//{
//    if (!strcmp(token.attribute.str.data, "Double")) {
//        return kw_Double;
//    }
//    else if (!strcmp(token.attribute.str.data, "else")) {
//        return kw_else;
//    }
//    else if (!strcmp(token.attribute.str.data, "func")) {
//        return kw_func;
//    }
//    else if (!strcmp(token.attribute.str.data, "if")) {
//        return kw_if;
//    }
//    else if (!strcmp(token.attribute.str.data, "Int")) {
//        return kw_Int;
//    }
//    else if (!strcmp(token.attribute.str.data, "let")) {
//        return kw_let;
//    }
//    else if (!strcmp(token.attribute.str.data, "nil")) {
//        return kw_nil;
//    }
//    else if (!strcmp(token.attribute.str.data, "return")) {
//        return kw_return;
//    }
//    else if (!strcmp(token.attribute.str.data, "String")) {
//        return kw_String;
//    }
//    else if (!strcmp(token.attribute.str.data, "while")) {
//        return kw_while;
//    }
//    else if (!strcmp(token.attribute.str.data, "var")) {
//        return kw_var;
//    }
//    else if (!strcmp(token.attribute.str.data, "while")) {
//        return kw_func;
//    }
//    else {
//        return kw_id;
//    }
//}


int handle_assignment(token_t token_assigner, global_symtab_t *global_table, local_symtab_w_par_ptr_t *local_table)
{
    token_t identifier;
    token_t var_type;
    bool is_constant = false;

    if (token_assigner.type == TOK_KW_LET) // let uvádí konstantní proměnnou
    {
        is_constant = true;
    }

    token_t current_token = getNextToken();
    identifier = current_token;

    if (identifier.type != TOK_IDENTIFIER) // musí následovat identifikátor
    {
        //error
    }

    current_token = getNextToken();

    if (current_token.type == TOK_COLON)
    {
        current_token = getNextToken();
        var_type = current_token;
        if (var_type.type != TOK_KW_DOUBLE && var_type.type != TOK_KW_INT && var_type.type != TOK_KW_STRING)
        {
            //error
        }
        current_token = getNextToken();
        if (current_token.type != TOK_EQUAL)
        {
            //error
        }
        // bottom-up parsing
    }
    else {
        if (current_token.type != TOK_EQUAL)
        {
            //error
            printf("error");
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
    current_token = getNextToken();

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
                current_token = getNextToken();
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
        current_token = getNextToken();
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

    token_t current_token = getNextToken();
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
                current_token = getNextToken();
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
        current_token = getNextToken();
    }
    return;
}

int find_functions()
{
    func_table_t func_table;
    token_t current_token;
    current_token = getNextToken();

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
                current_token = getNextToken();
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
            token_t func_name;
            current_token = getNextToken();
            if (current_token.type != TOK_IDENTIFIER)
            {
                // error
            }
            // funkce nesmi byt zabudovana/jiz definovana
            func_name = current_token;
            current_token = getNextToken();
            if (current_token.type != TOK_L_BRCKT)
            {
                // error
            }
            current_token = getNextToken();

            // TODO

            // zapis do tabulky funkci
        }
        current_token = getNextToken();
    }
}

int parse()
{
    find_functions();
    global_symtab_t *global_table;
    global_init(&global_table);
    //parse_block(0, global_table, NULL);
    global_dispose(&global_table);
    return 1;
}