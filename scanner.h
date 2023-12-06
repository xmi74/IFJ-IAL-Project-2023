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


#ifndef _SCANNER_H_
#define _SCANNER_H_

#include <stdbool.h>
#include "dynamic_string.h"
#include "error.h"

// Typ tokenu
typedef enum typ
{
    TOK_NOTHING,        // POCIATOCNY, PRAZDNY TOKEN

    TOK_IDENTIFIER,     // IDENTIFIKATOR 

    // KLUCOVE SLOVA
    TOK_KW_DOUBLE,
    TOK_KW_ELSE,
    TOK_KW_FUNC, 
    TOK_KW_IF,   
    TOK_KW_INT,   
    TOK_KW_LET,   
    TOK_KW_NIL,  
    TOK_KW_RETURN, 
    TOK_KW_STRING, 
    TOK_KW_VAR,  
    TOK_KW_WHILE, 

    TOK_UNDERSCORE,      // _
    TOK_EOF,             // END OF FILE
    TOK_INT,             // INT_LITERAL
    TOK_DOUBLE,          // DOUBLE_LITERAL

    TOK_STRING,

    TOK_MUL,             // * 
    TOK_DIV,             // / 
    TOK_PLUS,            // + 
    TOK_MINUS,           // - 

    TOK_R_CRL_BRCKT,     // } 
    TOK_L_CRL_BRCKT,     // { 
    TOK_R_BRCKT,         // ) 
    TOK_L_BRCKT,         // ( 
    TOK_ARROW,           // -> 

    TOK_NOT,             // ! 
    TOK_NOT_EQUAL,       // != 
    TOK_LESSER,          // < 
    TOK_LESSER_OR_EQUAL, // <=
    TOK_GREATER,         // > 
    TOK_GREATER_OR_EQUAL, // >=
    TOK_EQUAL,           // =
    TOK_ASSIGN,          // ==

    TOK_COLON,           // : 
    TOK_COMMA,           // , 
    TOK_SEMICLN,         // ;
    TOK_DOT,             // . 

    TOK_QUEST_MARK,      // ? 
    TOK_DOUBLE_QUEST_MARK, // ?? 

    TOK_COMMENT,         // // 
    TOK_BLOCK_COM_START, // /*
    TOK_BLOCK_COM_END,   // */  
    TOK_EOL              // END OF LINE
} token_type_t;

// Atribut tokenu
typedef struct // struct namiesto union
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
    struct ast_node *tree;
    bool terminal;
} token_t;


/**
 * @brief Nacitanie znaku zo vstupu
 * 
 * @return Integer hodnota znaku nacitaneho zo vstupu
*/
int getNextChar();

/**
 * @brief Vratenie nacitaneho znaku naspat na vstup
 * 
 * @param c Nacitany znak
*/
void ungetChar(int c);

/**
 * @brief Inicializacia tokenu
 * 
 * @param token Ukazatel na token
*/
void initToken(token_t *token);

// Uvolnenie pamati tokenu
/**
 * @brief Uvolnenie pamati tokenu
 * 
 * @param token ukazatel na token
*/
void freeToken(token_t *token);

// 
/**
 * @brief Hlavna funkcia lexikalneho analyzatora, priradzuje tokenu typ a atribut
 * 
 * @return token s priradenim typom, pripadne aj atributom
*/
token_t getNextToken();

#endif
