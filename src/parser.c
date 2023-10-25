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


int handle_assignment(token_t token_assigner, global_symtab_t *global_table, local_symtab_t *local_table)
{
    token_t identifier;
    token_t var_type;
    bool is_constant = false;

    if (token_assigner.type == TOK_KW_LET) // let uvádí konstantní proměnnou
    {
        is_constant = true;
    }

    token_t currentToken = getNextToken();
    identifier = currentToken;

    if (identifier.type != TOK_IDENTIFIER) // musí následovat identifikátor
    {
        //error
    }

    currentToken = getNextToken();

    if (currentToken.type == TOK_COLON)
    {
        currentToken = getNextToken();
        var_type = currentToken;
        if (var_type.type != TOK_KW_DOUBLE && var_type.type != TOK_KW_INT && var_type.type != TOK_KW_STRING)
        {
            //error
        }
        currentToken = getNextToken();
        if (currentToken.type != TOK_EQUAL)
        {
            //error
        }
        // bottom-up parsing
    }
    else {
        if (currentToken.type != TOK_EQUAL)
        {
            //error
            printf("error");
        }

       if (currentToken.type == TOK_SEMICLN) // nebo newline
       {
           //local_insert()
       }
    }

}

int parse_block(int nest_level, global_symtab_t *global_table, local_symtab_t *local_table_one_up)
{
    token_t currentToken;
    currentToken = getNextToken();

    local_symtab_t *local_table;
    local_init(&local_table);

    if (nest_level == 0) // main block
    {
        ;
    }

    while (currentToken.type != TOK_EOF) {
        if (currentToken.type == TOK_COMMENT)
        {
            ;
        }
        if (currentToken.type == TOK_BLOCK_COM_START)
        {
            int comment_nest_level = 1;
            while (comment_nest_level != 0)
            {
                currentToken = getNextToken();
                if (currentToken.type == TOK_BLOCK_COM_START)
                {
                    comment_nest_level++;
                }
                else if (currentToken.type == TOK_BLOCK_COM_END)
                {
                    comment_nest_level--;
                }
            }
            continue;
        }
        else if (currentToken.type == TOK_KW_LET || currentToken.type == TOK_KW_VAR)
        {
            handle_assignment(currentToken, global_table, local_table);
            
        }
        currentToken = getNextToken();
    }
}

int parse()
{
    global_symtab_t *global_table;
    global_init(&global_table);
    parse_block(0, global_table, NULL);
    global_dispose(&global_table);
    return 1;
}