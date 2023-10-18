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
    kw_id,
    kw_Double,
    kw_else,
    kw_func,
    kw_if,
    kw_Int,
    kw_let,
    kw_nil,
    kw_return,
    kw_String,
    kw_var,
    kw_while
} keyword_t;

keyword_t keyword_type(token_t token);

int parse();
