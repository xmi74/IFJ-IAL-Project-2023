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
    if (!strcmp(token.attribute.str.data, "func")) {
        return FUNC;
    }
    else if (!strcmp(token.attribute.str.data, "if")) {
        return IF;
    }
    else if (!strcmp(token.attribute.str.data, "else")) {
        return ELSE;
    }
    else if (!strcmp(token.attribute.str.data, "while")) {
        return WHILE;
    }
    else if (!strcmp(token.attribute.str.data, "for")) {
        return FOR;
    }
    else {
        return ID;
    }
}


int parse()
{
    token_t currentToken;
    currentToken = getNextToken();
    while (currentToken.type != TOK_EOF) {
        if (currentToken.type == TOK_COMMENT)
        {
            
        }
        else if (currentToken.type == TOK_IDENTIFIER)
        {
            keyword_type(currentToken);
        }
        

        currentToken = getNextToken();
    }
    return 1;
}