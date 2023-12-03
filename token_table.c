/**
 * IFJ-IAL Projekt 2023
 * 
 * @file token_table.c
 * @brief Implementacia programu syntakticke a semanticke analyzy
 * 
 * @author Igor Mikula (xmikul74)
 * @author Marko Olesak (xolesa00)
 * @author Jan Findra (xfindr01)
 * @author Tomas Arlt (xarltt00)
*/

#include "token_table.h"

extern token_table_t token_table;

void initTokenTable(token_table_t *table)
{
    table->insert = true;
    table->capacity = 64;
    table->size = 0;
    table->index = 0;
    table->tokens = malloc(table->capacity * sizeof(token_t));
    if (table->tokens == NULL)
    {
        // error
    }    
}

void freeTokenTable(token_table_t *table)
{
    free(table->tokens);
}

void addTableToken(token_table_t *table, token_t token)
{
    if (table->capacity<=table->size)
    {
        table->capacity *= 2;
        table->tokens = realloc(table->tokens, table->capacity * sizeof(token_t)); // asi fixnout memleak
        if (table->tokens == NULL)
        {
            // error
        }
    }
    table->tokens[table->size] = token;
    table->size++;
}

token_t getTableToken(token_table_t *table)
{
    if(table->size > table->capacity)
    {
        token_t eof_token;
        eof_token.type = TOK_EOF;
        return eof_token;
    }
    return table->tokens[table->index++];
}

token_t getTokenAssert(token_type_t type) // getToken, vraci chybu pokud obdrzi jiny token nez ocekavany
{
    token_t token = getToken();
    if (token.type != type)
    {
        // error
        returnError(SYNTAX_ERR); // TODO, placeholder
    }
    return token;
}

token_t getTokenAssertArr(int tok_amount, token_type_t *type) // getToken, vraci chybu pokud obdrzi jiny token nez ze seznamu ocekavanych
{
    token_t token = getToken();
    for(int i = 0; i < tok_amount; i++)
    {
        if (token.type == type[i])
        {
            return token;
        }
    }
    // error
    returnError(SYNTAX_ERR); // TODO, placeholder
    return token; // jen aby si kompilator nestezoval
}

token_t getToken()
{
    if(token_table.insert == true)
    {
        token_t token = getNextToken();
        addTableToken(&token_table, token);
        return token;
    }
    else
    {
        return getTableToken(&token_table);
    }
}

void ungetToken()
{
    token_table.index--;
}