/**
 * IFJ-IAL Projekt 2023
 * 
 * @file parser.h
 * @brief Implementacia programu syntakticke a semanticke analyzy
 * 
 * @author Igor Mikula (xmikul74)
 * @author Marko Olesak (xolesa00)
 * @author Jan Findra (xfindr01)
 * @author Tomas Arlt (xarltt00)
*/

#include "scanner.c"


typedef enum keyword_type
{
    ID,
    FUNC,
    IF,
    ELSE,
    WHILE,
    FOR
} keyword_t;

keyword_t keyword_type(token_t token);

int parse();
