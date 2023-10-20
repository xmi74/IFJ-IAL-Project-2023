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


keyword_t keyword_type(token_t token) // rozliseni identifikatoru a klicovych slov
{
    if (!strcmp(token.attribute.str.data, "Double")) {
        return kw_Double;
    }
    else if (!strcmp(token.attribute.str.data, "else")) {
        return kw_else;
    }
    else if (!strcmp(token.attribute.str.data, "func")) {
        return kw_func;
    }
    else if (!strcmp(token.attribute.str.data, "if")) {
        return kw_if;
    }
    else if (!strcmp(token.attribute.str.data, "Int")) {
        return kw_Int;
    }
    else if (!strcmp(token.attribute.str.data, "let")) {
        return kw_let;
    }
    else if (!strcmp(token.attribute.str.data, "nil")) {
        return kw_nil;
    }
    else if (!strcmp(token.attribute.str.data, "return")) {
        return kw_return;
    }
    else if (!strcmp(token.attribute.str.data, "String")) {
        return kw_String;
    }
    else if (!strcmp(token.attribute.str.data, "while")) {
        return kw_while;
    }
    else if (!strcmp(token.attribute.str.data, "var")) {
        return kw_var;
    }
    else if (!strcmp(token.attribute.str.data, "while")) {
        return kw_func;
    }
    else {
        return kw_id;
    }
}


int handle_assignment(token_t token_assigner, global_symtab_t *global_table, local_symtab_t *local_table)
{
    token_t currentToken = getNextToken();

    if (keyword_type(currentToken) != kw_id)
    {
        //error
    }

    if (currentToken.type == TOK_COLON)
    {
        currentToken = getNextToken();
        if (keyword_type(currentToken) != kw_Double && keyword_type(currentToken) != kw_Int && keyword_type(currentToken) != kw_String)
        {
            //error
        }
        currentToken = getNextToken();
        if (currentToken.type != TOK_ASSIGN)
        {
            //error
        }
        // bottom-up parsing
    }
    else {
        if (currentToken.type != TOK_ASSIGN)
        {
            //error
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
            while (currentToken.type != TOK_BLOCK_COM_END)
            {
                currentToken = getNextToken();
            }
            continue;
        }
        else if (currentToken.type == TOK_IDENTIFIER)
        {
            keyword_t currentKeyword = keyword_type(currentToken);
            if (currentKeyword == kw_let || currentKeyword == kw_var)
            {
                handle_assignment(currentToken, global_table, local_table);
            }
            
        }
        currentToken = getNextToken();
    }
}

int parse()
{
    global_symtab_t global_table;
    global_init(&global_table);
    parse_block(0, &global_table, NULL); // main block
    global_dispose(&global_table);
    return 1;
}