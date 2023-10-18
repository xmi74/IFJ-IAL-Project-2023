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


int parse()
{
    token_t currentToken;
    currentToken = getNextToken();
    while (currentToken.type != TOK_EOF) {
        if (currentToken.type == TOK_COMMENT)
        {
            ;
        }
        else if (currentToken.type == TOK_IDENTIFIER)
        {
            keyword_type(currentToken);

        }
        

        currentToken = getNextToken();
    }
    return 1;
}