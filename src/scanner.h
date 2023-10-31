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

#include "dynamic_string.h"
#include "error.h"

#ifndef _SCANNER_H_
#define _SCANNER_H_


// Typ tokenu
typedef enum typ
{
    TOK_NOTHING,          // POCIATOCNY, PRAZDNY TOKEN

    TOK_IDENTIFIER,       // IDENTIFIKATOR
                          // KLUCOVE SLOVA  
    TOK_KW_DOUBLE,        // Double 
    TOK_KW_ELSE,          // else
    TOK_KW_FUNC,          // func
    TOK_KW_IF,            // if
    TOK_KW_INT,           // Int
    TOK_KW_LET,           // let
    TOK_KW_NIL,           // nil
    TOK_KW_RETURN,        // return
    TOK_KW_STRING,        // String
    TOK_KW_VAR,           // var
    TOK_KW_WHILE,         // while

    TOK_EOF,              // END OF FILE
    TOK_INT,              // INTEGER
    TOK_DOUBLE,           // DOUBLE
    TOK_STRING_START,     // "
    TOK_STRING,           // STRING

    
    TOK_MUL,              // *
    TOK_DIV,              // /
    TOK_PLUS,             // +
    TOK_MINUS,            // -

    TOK_R_CRL_BRCKT,      // }
    TOK_L_CRL_BRCKT,      // {
    TOK_R_BRCKT,          // )
    TOK_L_BRCKT,          // (
    TOK_ARROW,            // ->

    TOK_NOT,              // !
    TOK_NOT_EQUAL,        // !=
    TOK_LESSER,           // <
    TOK_LESSER_OR_EQUAL,  // <=
    TOK_GREATER,          // >
    TOK_GREATER_OR_EQUAL, // >=
    TOK_EQUAL,            // =
    TOK_ASSIGN,           // ==

    TOK_COLON,            // :
    TOK_COMMA,            // ,
    TOK_SEMICLN,          // ;
    TOK_DOT,              // .

    TOK_QUEST_MARK,       // ?
    TOK_DOUBLE_QUEST_MARK,// ?

    TOK_COMMENT,          // //
    TOK_BLOCK_COM_START,  // /*
    TOK_BLOCK_COM_END,    // */    
    TOK_EOL               // \n
} token_type_t;

// Atribut tokenu
typedef union
{
    string_t str;
    double decimal;
    int number;
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
