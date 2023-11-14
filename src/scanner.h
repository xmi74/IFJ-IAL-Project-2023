/**
 * IFJ-IAL Projekt 2023
 * 
 * @file scanner.h
 * @brief Implementacia programu pre lexikalnu analyzu
 * 
 * @author Igor Mikula (xmikul74)
 * @author Marko Olesak (xolesa00)
 * @author Jan Findra (xfindr01)
 * @author Tomas Arlt (xarltt00)
*/

#include <stdbool.h>
#include "dynamic_string.h"
#include "error.h"

#ifndef _SCANNER_H_
#define _SCANNER_H_

// Identifikator - znak podtrzitka '_' , zaciatok zadania
// Escape Sekvencia
// Blokovy komentar - aby nebral text
// Volanie errorov

// Typ tokenu
typedef enum typ
{
    TOK_NOTHING,          // POCIATOCNY, PRAZDNY TOKEN

    TOK_IDENTIFIER,       // IDENTIFIKATOR X

    // KLUCOVE SLOVA  
    TOK_KW_DOUBLE,        // Double X
    TOK_KW_ELSE,          // else X
    TOK_KW_FUNC,          // func X
    TOK_KW_IF,            // if X
    TOK_KW_INT,           // Int X
    TOK_KW_LET,           // let X 
    TOK_KW_NIL,           // nil _ ??X??
    TOK_KW_RETURN,        // return X
    TOK_KW_STRING,        // String X
    TOK_KW_VAR,           // var X
    TOK_KW_WHILE,         // while X

    TOK_UNDERSCORE,       // _
    TOK_EOF,              // END OF FILE  X
    TOK_INT,              // INT_LITERAL X
    TOK_DOUBLE,           // DOUBLE_LITERAL X
    TOK_STRING_START,     // "
    TOK_STRING,           // STRING_LITERAL X
    
    TOK_MUL,              // * EX
    TOK_DIV,              // / EX
    TOK_PLUS,             // + EX
    TOK_MINUS,            // - EX

    TOK_R_CRL_BRCKT,      // } X
    TOK_L_CRL_BRCKT,      // { X
    TOK_R_BRCKT,          // ) X
    TOK_L_BRCKT,          // ( X
    TOK_ARROW,            // -> X

    TOK_NOT,              // ! EX
    TOK_NOT_EQUAL,        // != EX
    TOK_LESSER,           // < EX
    TOK_LESSER_OR_EQUAL,  // <= EX
    TOK_GREATER,          // > EX
    TOK_GREATER_OR_EQUAL, // >= EX
    TOK_EQUAL,            // = X
    TOK_ASSIGN,           // == EX

    TOK_COLON,            // : X
    TOK_COMMA,            // , X
    TOK_SEMICLN,          // ; NENI
    TOK_DOT,              // . NENI

    TOK_QUEST_MARK,       // ? X
    TOK_DOUBLE_QUEST_MARK,// ?? EX

    TOK_COMMENT,          // // X
    TOK_BLOCK_COM_START,  // /* X 
    TOK_BLOCK_COM_END,    // */  X
    TOK_EOL,
    
    TOK_EXPRESSION
} token_type_t;

// Atribut tokenu
typedef struct   // struct namiesto union
{
    string_t str;
    double doubleValue;
    int intValue;
    bool includesNil;
} token_attribute_t;

// Struktura tokenu (typ, atribut)
typedef struct
{
    token_attribute_t attribute;
    token_type_t type;
} token_t;


// Nacitanie znaku zo vstupu
int getNextChar();

// Vratenie znaku spat do vstupu
void ungetChar(int c);

// Inicializacia tokenu
void initToken(token_t *token);

// Uvolnenie pamati tokenu
void freeToken(token_t *token);

// Hlavna funkcia lexikalneho analyzatora, vracia token s priradenym typom a atributom
token_t getNextToken();

#endif
