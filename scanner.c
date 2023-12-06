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

void handleEscapeSequence(int *c)
{
    *c = getNextChar();
    switch (*c)
    {
        case '0':       // '\0' podla swift dokumentacie
            *c = '\0';
            break;
        case '\\':      // '\\'
            *c = '\\';
            break;
        case 't':       // '\t'
            *c = '\t';
            break;
        case 'n':       // '\n'
            *c = '\n';
            break;
        case 'r':       // '\r'
            *c = '\r';
            break;        
        case '\"':
            *c = '\"';
            break;
        case '\'':      // '\'' podla swift dokumentacie
            *c = '\'';
            break;
        case 'u':       // HEXADECIMAL
            if ((*c = getNextChar()) == '{')
            {
                int value = 0;
                while ((*c = getNextChar()) != '}')
                {
                    if (isxdigit(*c))
                    {
                        if (isdigit(*c)) 
                        {
                            value = (16 * value) + (*c - '0');
                        }
                        else
                        {
                            value = (16 * value) + (tolower(*c) - 'a' + 10);
                        }   
                    }
                    else
                    {
                        value = *c;
                        fprintf(stderr, "SCANNER: Escape sekvencia [ \\u ] nebola riadne ukoncena, ocakavana [ } ], problemovy znak: %c\n", value);
                        returnError(SCANNER_ERR);
                        break;
                    }
                }
                *c = value;                                                            
            }
            else
            {
                fprintf(stderr, "\nSCANNER: Ocakavany znak za escape sekvenciou \\u [ } ], ale je [ %c ]\n", *c);
                returnError(SCANNER_ERR);    
            }
            break;
        default:
            fprintf(stderr, "\nSCANNER: Znaky retazcoveho literalu [ \\%c ] nenabudaju ziadnu platnu escape sekvenciu\n", *c);
            returnError(SCANNER_ERR);
            break;
    }

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
    //int lineNumber = 1; // Cislo riadku
    token_t token;
    int c;

    initToken(&token);

    while (isspace(c = getNextChar())) { if (c == '\n') { ungetChar(c); break; }}
 
    /*  ------ SPRACOVANIE IDENTIFIER ------ */
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
            c = getNextChar();
            if (c == '?')
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
    /*  ------ SPRACOVANIE CISELNY LITERAL ------ */
    else if (isdigit(c))    // ciselny literal musi byt oddeleny medzerou od dalsieho znaku
    {
        char buffer[300];
        int i = 0;
        buffer[i++] = c;
        bool doubleHandled = false;

        while (doubleHandled == false &&(c = getNextChar()) != EOF) 
        {
            /*  ------ SPRACOVANIE DOUBLE LITERALU S DESATINNOU CIARKOU ------ */
            if (c == '.')   // ak dostanem bodku -> mozu nasledovat iba dalsie cisla
            {
                buffer[i++] = c;    // zapis . do bufferu
                c = getNextChar();
                if (isdigit(c))         // ak za . nasleduje cislo
                {
                    buffer[i++] = c;        
                    while ((c = getNextChar()) != EOF)
                    {
                        if (isdigit(c))     // ak je cislo, pripisuj dalej   ZA . MOZE BYT AJ DALSI EXPONENT
                        {
                            buffer[i++] = c;
                        }
                        else if (c == 'e' || c == 'E')   // ak dostanem e/E   -> moze nasledovat +/- alebo iba dalsie cisla
                        {
                            buffer[i++] = c;     // zapis e/E do bufferu
                            c = getNextChar();
                            if (c == '+' || c == '-')     // za exponentom moze nasledovat +/-
                            {
                                buffer[i++] = c;    
                                c = getNextChar();
                                if (isdigit(c))         // ak pride +/- tak musi nasledovat aspon 1 cislo
                                {
                                    buffer[i++] = c;
                                    while ((c = getNextChar()) != EOF)
                                    {
                                        if (isdigit(c))     // ak je cislo, pripisuj dalej
                                        {
                                            buffer[i++] = c;
                                        }
                                        else                // ak nie je cislo, error
                                        {
                                            ungetChar(c);
                                            doubleHandled = true;
                                            break;
                                        }
                                    }
                                }
                                else    // ak za exponentom nenasleduje aspon 1 cislo -> error
                                {
                                    fprintf(stderr, "SCANNER: Za Exponentom DOUBLE literalu nasledovaneho znamienkom musi nasledovat aspon 1 cislo\n");
                                    returnError(SCANNER_ERR);
                                }
                                
                            }
                            else if (isdigit(c))    // ak za Exponentom prislo cislo bez znamienka, je automaticky kladny
                            {
                                buffer[i++] = c;
                                while ((c = getNextChar()) != EOF)
                                {
                                    if (isdigit(c))     // ak je cislo, pripisuj dalej
                                    {
                                        buffer[i++] = c;
                                    }
                                    else                // ak nie je cislo, ani operator
                                    {
                                        ungetChar(c);
                                        doubleHandled = true;   // nove?
                                        break;
                                    }
                                }
                            }
                            else    // ak za exponentom nasleduje hocico ine ako +/-/cislo -> error
                            {
                                fprintf(stderr, "\nSCANNER: Za exponentom DOUBLE literalu musi nasledovat +/-/cislo : [ %c ]\n", c);
                                returnError(SCANNER_ERR);
                            }                
                        }
                        else
                        {
                            doubleHandled = true;
                            break;                            
                        }                        
                    }
                }
                else                    // ak za . nenasleduje cislo -> error
                {
                    fprintf(stderr, "\nSCANNER: Chyba pri zapise do double literalu ! za '.' musi nasledovat cislo\n");
                    returnError(SCANNER_ERR);
                }
            }             
            /*  ------ SPRACOVANIE DOUBLE LITERALU S EXPONENTOM ------ */
            else if (c == 'e' || c == 'E')   // ak dostanem e/E   -> moze nasledovat +/- alebo iba dalsie cisla
            {
                buffer[i++] = c;     // zapis e/E do bufferu
                c = getNextChar();
                if (c == '+' || c == '-')     // za exponentom moze nasledovat +/-
                {
                    buffer[i++] = c;    
                    c = getNextChar();
                    if (isdigit(c))         // ak pride +/- tak musi nasledovat aspon 1 cislo
                    {
                        buffer[i++] = c;
                        while ((c = getNextChar()) != EOF)
                        {
                            if (isdigit(c))     // ak je cislo, pripisuj dalej
                            {
                                buffer[i++] = c;
                            }
                            else                // ak nie je cislo, error
                            {
                                doubleHandled = true;
                                break;
                            }
                        }
                    }
                    else    // ak za exponentom nenasleduje aspon 1 cislo -> error
                    {
                        fprintf(stderr, "SCANNER: Za Exponentom DOUBLE literalu nasledovaneho znamienkom musi nasledovat aspon 1 cislo\n");
                        returnError(SCANNER_ERR);
                    }
                    
                }
                else if (isdigit(c))    // ak za Exponentom prislo cislo bez znamienka, je automaticky kladny
                {
                    buffer[i++] = c;
                    while ((c = getNextChar()) != EOF)
                    {
                        if (isdigit(c))     // ak je cislo, pripisuj dalej
                        {
                            buffer[i++] = c;
                        }
                        else                // ak nie je cislo, ani operator
                        {
                            ungetChar(c);
                            break;
                        }
                    }
                }
                else    // ak za exponentom nasleduje hocico ine ako +/-/cislo -> error
                {
                    fprintf(stderr, "\nSCANNER: Za exponentom DOUBLE literalu musi nasledovat +/-/cislo : [ %c ]\n", c);
                    returnError(SCANNER_ERR);
                }                
            }
            /*  ------ SPRACOVANIE INTEGERU CI DOUBLE PRED ZISKANIM EXPONENTU ALEBO DESATINNEJ CIARKY ------ */
            else if (isdigit(c))
            {
                buffer[i++] = c;
            }
            else
            {
                break;
            }
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
    /*  ------ SPRACOVANIE STRINGOV ------ */
    else if (c == '"')                              // STRING_LITERAL
    {
        string_t string;
        dstringInit(&string);
        c = getNextChar();        

        if (c == '"')           // "" prazdny retazec
        {
            c = getNextChar();
            if (c == '"')       // """ viacradkovy retazec
            {
                bool stringFinished = false;
                c = getNextChar();      
                if (c == '\n')
                {
                    ;   // nerob nic
                }
                else
                {
                    fprintf(stderr, "SCANNER: Chybajuce odriadkovanie za inicializaciou [ \"\"\" ] viacriadkoveho retazca\n ");  // Je to error?
                    returnError(SCANNER_ERR);
                }
                while ((c = getNextChar()) != EOF)  // Som vo viacriadkovom retazci, za prvym odriadkovanim a zacinam nacitavat znaky
                {
                    if (c == '\n')  // ak narazim na \n a za nim nasleduje "
                    {
                        int newLine = c;
                        c = getNextChar();                                          
                        if (c == '"')   // chceme ukoncit string alebo zapisat 1x "
                        {
                            int firstQuotation = c;
                            c = getNextChar();
                            if (c == '"')       // chceme ukoncit string alebo zapisat 2x "
                            {
                                int SecondQuotation = c;
                                c = getNextChar();
                                if (c == '"')       // chceme ukoncit string
                                {             
                                    stringFinished = true;               
                                    break;
                                }
                                else            // nechceme ukoncit string
                                {
                                    //printf("Appendujem newLine, firstQuotation, secondQuotation, char\n");
                                    dstringAppend(&string, newLine);            // zapis newLine
                                    dstringAppend(&string, firstQuotation);     // zapis 1. "
                                    dstringAppend(&string, SecondQuotation);    // zapis 2. "
                                    //printf("2. \" : zapisujem znak : [ %c ]\n", c);
                                    ungetChar(c);
                                    //dstringAppend(&string, c);                  // zapis znaku
                                }
                            }
                            else
                            {
                                //printf("Appendujem newLine, firstQuotation, char\n");
                                dstringAppend(&string, newLine);
                                dstringAppend(&string, firstQuotation);
                                //printf("1. \" : zapisujem znak : [ %c ]\n", c);
                                ungetChar(c);
                                //dstringAppend(&string, c);                            
                            }
                        }
                        else
                        {
                            //printf("Appendujem newLine, %c\n", c);
                            dstringAppend(&string, newLine);
                            ungetChar(c);
                            //dstringAppend(&string, c);
                        }
                    }
                    else if (c == '\\')              // Escape sekvencia
                    {
                        handleEscapeSequence(&c);
                        dstringAppend(&string, c);
                    }
                    else
                    {
                        //printf("Appendujem %c\n", c);
                        dstringAppend(&string, c);
                    }
                    
                }
                if (stringFinished == false)
                {
                    fprintf(stderr, "SCANNER: Viacriadkovy string nebol riadne ukonceny (Chybajuce [ \" ] alebo odriadkovanie!\n");
                    returnError(SCANNER_ERR);
                }
            }           // """ koniec viacriadkoveho retazca
            else
            {
                int emptyStringChar = 0;
                dstringAppend(&string, emptyStringChar); // a do retazca zapis prazdny retazec
                ungetChar(c);               // vrat znak 
            }
        }
        else                    // " klasicky string
        {
            while (c != EOF) 
            {
                if (c == '\\')              
                {
                    handleEscapeSequence(&c);
               
                }
                else if (c <= 31)
                {
                    fprintf(stderr, "SCANNER: Zapis nepovoleneho znaku do stringu! (ASCII hodnota < 31 a neobsahuje escape sekvenciu)\n");
                    returnError(SCANNER_ERR);
                }
                else if (c == '"')
                {
                    break;
                }
                //printf("pridavam do stringu znak : [ %d ]\n", c);
                dstringAppend(&string, c);
                c = getNextChar();
            }
            if (c != '"')
            {
                fprintf(stderr, "SCANNER: Neukonceny klasicky string! posledny znak : [ %c ]\n", c);
                returnError(SCANNER_ERR);
            }
        }
        
        //ungetChar(c);
        token.type = TOK_STRING;
        token.attribute.str = string;   // neni potreba dstring copy ?
    }
    // FSM - DONE od dola
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
        if (c == '*')   // Blokovy komentar /* ... */
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
                fprintf(stderr, "SCANNER: Chybaju [ %d ] ukoncenia blokovych komentarov\n", nestedCommentLevel);
                returnError(SCANNER_ERR);
            }
        }
        else if (c == '/')  // Riadkovy koment // ...
        {
            token.type = TOK_COMMENT;
            while ((c = getNextChar()) != EOF && c != '\n')
            {
                // V riadkovom komentari ignoruj znaky az do konca riadku
            }
        }
        else
        {
            ungetChar(c); 
        }
    }
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
    else if (c == '?')
    {
        c = getNextChar();
        if (c == '?')
        {
            token.type = TOK_DOUBLE_QUEST_MARK;     // ??
        }
        else
        {
            fprintf(stderr, "\nSCANNER: Nemozno nacitat samotny [ ? ]\n");
            returnError(SCANNER_ERR);
        }
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
    else if (c == '}') token.type = TOK_R_CRL_BRCKT;// }
    else if (c == '{') token.type = TOK_L_CRL_BRCKT;// {
    else if (c == ')') token.type = TOK_R_BRCKT;    // )
    else if (c == '(') token.type = TOK_L_BRCKT;    // (
    else if (c == ':') token.type = TOK_COLON;      // ,       
    else if (c == ',') token.type = TOK_COMMA;      // ,
    else if (c == EOF) token.type = TOK_EOF;        // EOF
    else if (c == '+') token.type = TOK_PLUS;       // +
    /*  ------------------------------------ */
    else
    {
        fprintf(stderr, "\nSCANNER: Nacitanie neznameho symbolu: [ %c ]\n", c);
        returnError(SCANNER_ERR);
    }

    return token;
}
