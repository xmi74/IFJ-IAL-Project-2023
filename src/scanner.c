/**
 * IFJ-IAL Projekt 2023
 * 
 * @file scanner.c
 * @brief Implementacia programu pre lexikalnu analyzu
 * 
 * @author Igor Mikula (xmikul74)
 * @author Marko Olesak (xolesa00)
 * @author Jan Findra (xfindr01)
 * @author
*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#include "scanner.h"


#define STATE_START 1

#define STATE_IDENTIFIER 2

#define STATE_EOF 3
#define STATE_INT 4
#define STATE_FLOAT 5

#define STATE_EQUAL 6
#define STATE_NOT_EQUAL 7
#define STATE_LESSER 8
#define STATE_LESSER_OR_EQUAL 9
#define STATE_GREATER 10
#define STATE_GREATER_OR_EQUAL 11
#define STATE_ASSIGN 12
#define STATE_NOT 13

#define STATE_PLUS 14
#define STATE_MINUS 15
#define STATE_MUL 16
#define STATE_DIV 17

#define STATE_R_BRCKT 18
#define STATE_L_BRCKT 19
#define STATE_R_CRL_BRCKT 20
#define STATE_L_CRL_BRCKT 21

#define STATE_CONCAT 22
#define STATE_COMMA 23
#define STATE_CLN 24
#define STATE_SEMICLN 25

// Nacitanie znaku zo vstupu
int getNextChar() 
{
    return getchar();
}

// Vratenie znaku spat do vstupu
void ungetChar(int c) 
{
    ungetc(c, stdin);
}

// Inicializacia tokenu
void initToken(token_t *token)
{
    token->type = TOK_NOTHING;
    memset(&token->attribute, 0, sizeof(token->attribute));
}

// Uvolnenie pamati tokenu
void freeToken(token_t *token) 
{
    if (token->type == TOK_IDENTIFIER) 
    {
        dstringFree(&(token->attribute.str));
    }
}

// Hlavna funkcia lexikalneho analyzatora, vracia token s priradenym typom a atributom
token_t getNextToken() 
{
    token_t token;
    int c;

    initToken(&token);

    while (isspace(c = getNextChar())) {}

    if (c == EOF)                       // EOF
    {
        token.type = TOK_EOF;
    } 
    else if (isalpha(c) || c == '_')    // IDENTIFIER
    {
        string_t identifier;
        dstringInit(&identifier);
        dstringAppend(&identifier, c);

        while ((c = getNextChar()) != EOF && (isalnum(c) || c == '_')) 
        {
            dstringAppend(&identifier, c);
        }
        ungetChar(c);

        token.type = TOK_IDENTIFIER;
        token.attribute.str = identifier;
    } 
    else if (isdigit(c) || c == '-')    // FLOAT
    {
        char buffer[100];
        int i = 0;
        buffer[i++] = c;
        while ((c = getNextChar()) != EOF && (isdigit(c) || c == '.')) 
        {
            buffer[i++] = c;
        }
        buffer[i] = '\0';
        ungetChar(c);

        if (strchr(buffer, '.') != NULL) 
        {
            token.type = TOK_FLOAT;
            token.attribute.decimal = atof(buffer);
        } else 
        {
            token.type = TOK_INT;
            token.attribute.number = atoi(buffer);
        }
    } 
    else if (c == '=') token.type = TOK_EQUAL;  // =
    else if (c == '!')                      // !
    {
        c = getNextChar();
        if (c == '=')                       // !=
        {
            token.type = TOK_NOT_EQUAL;
        } else 
        {
            ungetChar(c);
        }
    } 
    else if (c == '<')                      // <
    {
        c = getNextChar();
        if (c == '=') 
        {
            token.type = TOK_LESSER_OR_EQUAL;   // <=
        } else 
        {
            ungetChar(c);
            token.type = TOK_LESSER;
        }
    } 
    else if (c == '>') token.type = TOK_GREATER;            
    else if (c == '+') token.type = TOK_PLUS;        
    else if (c == '-') token.type = TOK_MINUS;    

    return token;
}
