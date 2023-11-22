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

/* TODO
    • Prevod float -> double    HOTOVO
    • Pridanie exponentu v double   HOTOVO
    • TOK_MINUS sa neda nacitat bez medzery, TOK_PLUS ano   HOTOVO
    • TOK_MUL nacitavanie   HOTOVO
    • TOK_EQUAL nacitavanie   HOTOVO
    • Ako je to s medzerami? musia ci nemusia byt? napr. 2+3    je mozne implementovat aj bez medzier - HOTOVO
    • Nenacitava / teda (TOK_DIV) !!!   HOTOVO
    • Kontrola vnorenych blokovych komentarov   ak nie je dostatok symbolov ukoncenia blok. kom.,potom error - HOTOVO
    • Kontrola kedy vzdy moze nastat error  HOTOVO
        -- Ked nacitavam neznamy znak
        -- Ked nacitam samotny ?
        -- Ked nie je dostatok ukonceni blok. komentaru
        -- Neocakavana escape sekvencia, pri hexadecimalnych - zly zapis escape sekvencie

    • Kontrola funkcnosti escape sekvencie
    • Viacriadkovy string """
*/

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

// Funkcia rozhodne ci ide o identifikator alebo klucove slovo
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
    else if (isdigit(c)) 
    {
        char buffer[100];
        int i = 0;
        buffer[i++] = c;

        while ((c = getNextChar()) != EOF && (isdigit(c) || c == '.' || c == 'e' || c == 'E' || c == '+' || c == '-')) 
        {
            buffer[i++] = c;
        }

        buffer[i] = '\0';
        ungetChar(c);

        if (strchr(buffer, '.') != NULL || strchr(buffer, 'e') != NULL || strchr(buffer, 'E') != NULL) 
        {
            token.type = TOK_DOUBLE;
            token.attribute.doubleValue = strtod(buffer, NULL);
        } 
        else 
        {
            token.type = TOK_INT;
            token.attribute.intValue = atoi(buffer);
        }
    }
    else if (c == '*')  // tu sa spracovava koniec blokoveho komentaru ?
    {
        token.type = TOK_MUL;
        c = getNextChar();
        if (c == '/')                               // */
        {
            token.type = TOK_BLOCK_COM_END;         
        }
        else                       // *
        {
            ungetChar(c);           
        }   
    }
    else if (c == '/')  // tu sa spracovava zaciatok blokoveho komentaru
    {
        token.type = TOK_DIV;
        c = getNextChar();
        if (c == '*') // Blokový komentár /* ... */
        {
            token.type = TOK_BLOCK_COM_START;
            int nestedCommentLevel = 1;

            while ((c = getNextChar()) != EOF)
            {
                if (c == '/' && (c = getNextChar()) == '*')
                {
                    nestedCommentLevel++;
                }
                else if (c == '*' && (c = getNextChar()) == '/')
                {
                    nestedCommentLevel--;
                    if (nestedCommentLevel == 0)
                    {
                        ungetChar('/');
                        ungetChar('*');
                        break;
                    }
                }
            }

            if (nestedCommentLevel > 0)
            {
                // Chyba: Neočakávaný koniec súboru v blokovom komentári
                fprintf(stderr, "Chybaju [ %d ] ukoncenia blokovych komentarov\n", nestedCommentLevel);
                returnError(SCANNER_ERR);
            }
        }
        else if (c == '/') // Riadkový komentár // ...
        {
            token.type = TOK_COMMENT;
            while ((c = getNextChar()) != EOF && c != '\n')
            {
                // Sme v komentári, teda nepotrebujeme načítavať znaky až do konca riadku
            }
        }
        else
        {
            ungetChar(c); // Vrátiť nečítaný znak späť do vstupu
        }
    }
    else if (c == EOF) token.type = TOK_EOF;        // EOF
    else if (c == '+') token.type = TOK_PLUS;       // +
    else if (c == '-')
    {
        token.type = TOK_MINUS;                 
        c = getNextChar();
        if (c == '>')                               // ->
        {
            token.type = TOK_ARROW;
        }
        else
        {
            ungetChar(c);
        } 
    }
    else if (c == '}') token.type = TOK_R_CRL_BRCKT;// }
    else if (c == '{') token.type = TOK_L_CRL_BRCKT;// {
    else if (c == ')') token.type = TOK_R_BRCKT;    // )
    else if (c == '(') token.type = TOK_L_BRCKT;    // (
    else if (c == '!')                              // !
    {
        token.type = TOK_NOT;
        c = getNextChar();
        if (c == '=')                               // !=
        {
            token.type = TOK_NOT_EQUAL;
        } 
        else 
        {
            ungetChar(c);
        }
    } 
    else if (c == '<')                              
    {
        token.type = TOK_LESSER;
        c = getNextChar();
        if (c == '=')                               // <=
        {
            token.type = TOK_LESSER_OR_EQUAL;       
        } 
        else 
        {
            ungetChar(c);                          
        }
    }
    else if (c == '>')
    {
        token.type = TOK_GREATER;               // >
        c = getNextChar();
        if (c == '=')
        {
             token.type = TOK_GREATER_OR_EQUAL;     // >=
        }
        else
        {
            ungetChar(c);                           
        }
    }
    else if (c == '=')
    {
        token.type = TOK_ASSIGN;                 // =
        c = getNextChar();
        if (c == '=')
        {
            token.type = TOK_EQUAL;                // ==
        }
        else
        {
            ungetChar(c);                           
        }
    }
    else if (c == ':') token.type = TOK_COLON;      // ,       
    else if (c == ',') token.type = TOK_COMMA;      // ,
    else if (c == '?')
    {
        c = getNextChar();
        if (c == '?')
        {
            token.type = TOK_DOUBLE_QUEST_MARK;     // ??
        }
        else
        {
            fprintf(stderr, "Nemozno nacitat samotny ?\n");
            returnError(SCANNER_ERR);
        }
    }
    else if (c == '"')                              // STRING_LITERAL (mozno bude treba dalsie upravy)
    {
        c = getNextChar();
        /*if (c == '"' && (c = getNextChar()) == '"')   // Viacriadkovy string
        {
            while ((c = getNextChar()) != EOF)
            {
                if (c == '"' && (c = getNextChar()) == '"' && (c = getNextChar()) == '"')
                {
                    break;
                }
            }
        }*/

        string_t string;
        dstringInit(&string);
        dstringAppend(&string, c);

        while ((c = getNextChar()) != EOF && c != '"') 
        {
            if (c == '\\')              // Escape sekvencia // Mozno vhodne vytvorit funkciu a zapisat ju aj do viacriadkoveho stringu
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
                    case '\"':
                        c = '\"';
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
                                else    // /u{nehexadecimal
                                {
                                    value = c;
                                    break;
                                }
                            }
                            c = value;                                                            
                        }
                        else
                        {
                            fprintf(stderr, "\nOcakavany znak za escape sekvenciou \\u [ } ], ale je [ %c ]\n", c);
                            returnError(SCANNER_ERR);    
                        }
                        break;
                    default:
                        fprintf(stderr, "\nZnaky retazcoveho literalu [ \\%c ] nenabudaju ziadnu platnu escape sekvenciu\n", c);
                        returnError(SCANNER_ERR);
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
        fprintf(stderr, "\nNacitanie neznameho symbolu: %c\n", c);
        returnError(SCANNER_ERR);
    }

    return token;
}
