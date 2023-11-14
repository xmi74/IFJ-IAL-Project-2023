/**
 * IFJ-IAL Projekt 2023
 * 
 * @file scanner.c
 * @brief Implementacia programu pre lexikalnu analyzu
 * 
 * @author Igor Mikula (xmikul74)
 * @author Marko Olesak (xolesa00)
 * @author Jan Findra (xfindr01)
 * @author Tomas Arlt (xarltt00)
*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#include "scanner.h"

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
    dstringFree(&(token->attribute.str));
}

void assignIdentifier(token_t *token, string_t identifier)
{
    if (strcmp(identifier.data, "Double") == 0) 
    {
        token->type = TOK_KW_DOUBLE;
    }
    else if (strcmp(identifier.data, "else") == 0) 
    {
        token->type = TOK_KW_ELSE;
    }
    else if (strcmp(identifier.data, "func") == 0) 
    {
        token->type = TOK_KW_FUNC;
    }
    else if (strcmp(identifier.data, "if") == 0) 
    {
        token->type = TOK_KW_IF;
    }
    else if (strcmp(identifier.data, "Int") == 0)   //
    {
        token->type = TOK_KW_INT;
    }
    else if (strcmp(identifier.data, "let") == 0) 
    {
        token->type = TOK_KW_LET;
    }
    else if (strcmp(identifier.data, "nil") == 0) 
    {
        token->type = TOK_KW_NIL;
    }
    else if (strcmp(identifier.data, "return") == 0) 
    {
        token->type = TOK_KW_RETURN;
    }
    else if (strcmp(identifier.data, "String") == 0) 
    {
        token->type = TOK_KW_STRING;
    }
    else if (strcmp(identifier.data, "_") == 0) // samotne _ nie je ID, __ uz je ID
    {
        token->type = TOK_UNDERSCORE;
    }
    else if (strcmp(identifier.data, "var") == 0) 
    {
        token->type = TOK_KW_VAR;
    }
    else if (strcmp(identifier.data, "while") == 0) 
    {
        token->type = TOK_KW_WHILE;
    }
    else
    {
        token->type = TOK_IDENTIFIER;
    }
    token->attribute.str = identifier;  // aj klucove slova budu obsahovat atribut
}

// Hlavna funkcia lexikalneho analyzatora, vracia token s priradenym typom a atributom
token_t getNextToken() 
{
    token_t token;
    int c;

    initToken(&token);

    while (isspace(c = getNextChar())) {}
 
    if (isalpha(c) || c == '_')    // IDENTIFIER
    {
        string_t identifier;
        dstringInit(&identifier);
        dstringAppend(&identifier, c);

        while ((c = getNextChar()) != EOF && (isalnum(c) || c == '_')) 
        {
            dstringAppend(&identifier, c);
        }
        ungetChar(c);
        assignIdentifier(&token, identifier);
        if (token.type == TOK_KW_DOUBLE || token.type == TOK_KW_INT || token.type == TOK_KW_STRING)
        {
            if ((c = getNextChar()) == '?')
            {
                token.attribute.includesNil = true;
            }
            else
            {
                ungetChar(c);
                token.attribute.includesNil = false;
            }
        }
        
    } 
    else if (isdigit(c))                // INT/DOUBLE   uklada hodnoty aj do token.attribute.str.data
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
            token.type = TOK_DOUBLE;
            token.attribute.doubleValue = atof(buffer);//strtod ! ! !
        } 
        else 
        {
            token.type = TOK_INT;
            token.attribute.intValue = atoi(buffer);
        }
    }
    else if (c == '*')
    {
        c = getNextChar();
        if (c == '/')                               // */
        {
            token.type = TOK_BLOCK_COM_END;         
        }
        else if (isspace(c))                        // *
        {
            token.type = TOK_MUL;           
        }
        else                                        // ERROR
        {
            ungetChar(c);
            fprintf(stderr, "Pri nacitavani *\n");
            returnError(SCANNER_ERR);
            //GLOBAL_ERROR_VALUE = SCANNER_ERR;
            
        }
    }
    else if (c == '/')
    {
        c = getNextChar();
        if (c == '*')                               // /*
        {
            token.type = TOK_BLOCK_COM_START;
            while ((c = getNextChar()) != EOF) 
            {
                if (c == '*' && (c = getNextChar()) == '/')
                {
                    ungetChar('/');
                    ungetChar('*');
                    break;
                }
            }
        }
        else if (c == '/')                          // //
        {
            token.type = TOK_COMMENT;
            while ((c = getNextChar()) != EOF && c != '\n') 
            {
                // Sme v komentari, teda nepotrebujeme nacitavat znaky az do konca riadku
            }        
        }
        else if (isspace(c))                        // /
        {
            token.type = TOK_DIV;
        }
        else
        {
            ungetChar(c);                           // ERROR
            returnError(SCANNER_ERR);
        }
    }
    else if (c == EOF) token.type = TOK_EOF;        // EOF
    else if (c == '+') token.type = TOK_PLUS;       // +
    else if (c == '-')
    {
        c = getNextChar();
        if (c == '>')                               // ->
        {
            token.type = TOK_ARROW;
        } 
        else if (isspace(c))                        // -
        {
            token.type = TOK_MINUS;                 
        }
        else
        {
            ungetChar(c);                           // ERROR
            returnError(SCANNER_ERR);
            //GLOBAL_ERROR_VALUE = 1;
        }
    }
    else if (c == '}') token.type = TOK_R_CRL_BRCKT;// }
    else if (c == '{') token.type = TOK_L_CRL_BRCKT;// {
    else if (c == ')') token.type = TOK_R_BRCKT;    // )
    else if (c == '(') token.type = TOK_L_BRCKT;    // (
    else if (c == '!')                              // !
    {
        c = getNextChar();
        if (c == '=')                               // !=
        {
            token.type = TOK_NOT_EQUAL;
        } 
        else if (isspace(c))                        // !
        {
            token.type = TOK_NOT;   
        }
        else
        {
            ungetChar(c);                           // ERROR
            returnError(SCANNER_ERR);
            //GLOBAL_ERROR_VALUE = 1;
        }
    } 
    else if (c == '<')                              
    {
        c = getNextChar();
        if (c == '=')                               // <=
        {
            token.type = TOK_LESSER_OR_EQUAL;       
        } 
        else if (isspace(c))                        // <
        {
            token.type = TOK_LESSER;
        }
        else 
        {
            ungetChar(c);                           // ERROR
            returnError(SCANNER_ERR);
        }
    }
    else if (c == '>')
    {
        c = getNextChar();
        if (c == '=')
        {
             token.type = TOK_GREATER_OR_EQUAL;     // >=
        }
        else if (isspace(c))
        {
            token.type = TOK_GREATER;               // >
        }
        else
        {
            //ungetChar(c);                           // ERROR
            returnError(SCANNER_ERR);
        }
    }
    else if (c == '=')
    {
        c = getNextChar();
        if (c == '=')
        {
            token.type = TOK_ASSIGN;                // ==
        }
        else if (isspace(c))
        {
            token.type = TOK_EQUAL;                 // =
        }
        else
        {
            //ungetChar(c);                           // ERROR
            returnError(SCANNER_ERR);
        }
    }
    else if (c == ':') token.type = TOK_COLON;      // ,       
    else if (c == ',') token.type = TOK_COMMA;      // ,
    else if (c == ';') token.type = TOK_SEMICLN;    // ;
    else if (c == '.') token.type = TOK_DOT;        // .
    else if (c == '?')
    {
        c = getNextChar();
        if (c == '?')
        {
            token.type = TOK_DOUBLE_QUEST_MARK;     // ??
        }
        else if (isspace(c))
        {
            //token.type = TOK_QUEST_MARK;            // ? ZMAZENIE, SAMOTNY QUEST_MARK NEEXISTUJE
            fprintf(stderr, "Spracovanie ?\n");
            returnError(SCANNER_ERR);
        }
        else
        {
            ungetChar(c);                           // ERROR
            returnError(SCANNER_ERR);
        }
    }
    else if (c == '"')                              // STRING_LITERAL (mozno bude treba dalsie upravy)
    {
        c = getNextChar();

        string_t string;
        dstringInit(&string);
        dstringAppend(&string, c);

        while ((c = getNextChar()) != EOF && c != '"') 
        {
            if (c == '\\')              // Escape sekvencia
            {
                c = getNextChar();
                switch (c)
                {
                    case 'n':       // '\n'
                        c = '\n';
                        break;
                    case 'r':       // '\r'
                        c = '\r';
                        break;
                    case 't':       // '\t'
                        c = '\t';
                        break;
                    case '\\':      // '\\'
                        c = '\\';
                        break;  
                    case 'u':       // HEXADECIMAL
                        if ((c = getNextChar()) == '{')
                        {
                            int value = 0;
                            while ((c = getNextChar()) != '}')
                            {
                                if (isxdigit(c))    // kod z https://copyprogramming.com/howto/how-to-convert-hex-to-ascii-in-c-with-and-without-using-sprintf
                                {
                                    if (isdigit(c)) 
                                    {
                                        value = (16 * value) + (c - '0');
                                    }
                                    else
                                    {
                                        value = (16 * value) + (tolower(c) - 'a' + 10);
                                    }   
                                }
                                else    // /u{G -> pokracuj
                                {
                                    value = c;
                                    break;
                                }
                            }
                            c = value;                                                            
                        }
                        break;
                    default:
                        break;
                }
            }   
            dstringAppend(&string, c);
        }
        //ungetChar(c);
        token.type = TOK_STRING;
        token.attribute.str = string;
    }
    else if (c == '\n')
    {
        token.type = TOK_EOL;                       // EOL

        c = getNextChar();
        while (c == '\n' || isspace(c))
        {
            c = getNextChar();
        }
        ungetChar(c);                               // ERROR
    }
    else
    {
        fprintf(stderr, "Nacitanie neznameho tokenu\n");
        returnError(SCANNER_ERR);
    }

    return token;
}
