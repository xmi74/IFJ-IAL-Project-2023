/**
 * IFJ-IAL Projekt 2023
 * 
 * @file token_table.h
 * @brief Implementacia tabulky spracovanych tokenov
 * 
 * @author Igor Mikula (xmikul74)
 * @author Marko Olesak (xolesa00)
 * @author Jan Findra (xfindr01)
 * @author Tomas Arlt (xarltt00)
*/

#ifndef TOKEN_TABLE_H
#define TOKEN_TABLE_H

#include "symtable.h"
#include <stdbool.h>

// Struktura tabulky tokenu
typedef struct tkn_tbl
{
    token_t *tokens;
    int size;
    int capacity;
    int index;
    bool insert; // vkladani/cteni
} token_table_t;

/**
 * @brief Inicializace tabulky tokenu.
 *
 * @param table Ukazatel na tabulku tokenu.
 */
void initTokenTable(token_table_t *table);

/**
 * @brief Dealokuje tabulku tokenu.
 *
 * @param table Ukazatel na tabulku tokenu.
 */
void freeTokenTable(token_table_t *table);

/**
 * @brief Prida token do tabulky tokenu.
 *
 * @param table Ukazatel na tabulku tokenu.
 * @param token Token, ktery se ma pridat do tabulky tokenu.
 */
void addTableToken(token_table_t *table, token_t token);

/**
 * @brief Vrati token z tabulky tokenu.
 *
 * @param table Ukazatel na tabulku tokenu.
 * @return Token z tabulky tokenu.
 */
token_t getTableToken(token_table_t *table);

/**
 * @brief Vrati token z tabulky tokenu.
 *
 * @return Token z tabulky tokenu.
 */
token_t getToken();

/**
 * @brief Vrati token z tabulky tokenu a zaroven overi, zda se jedna o ocekavany token.
 *
 * @param type Ocekavany typ tokenu.
 * @param err_code Chybovy kod, ktery se ma vratit v pripade, ze se nejedna o ocekavany token.
 * @return Token z tabulky tokenu.
 */
token_t getTokenAssert(token_type_t type, int err_code);

/**
 * @brief Vrati token z tabulky tokenu a zaroven overi, zda se jedna o ocekavany token (ze seznamu).
 *
 * @param tok_amount Pocet ocekavanych tokenu.
 * @param type Seznam ocekavanych typu tokenu.
 * @param err_code Chybovy kod, ktery se ma vratit v pripade, ze se nejedna o ocekavany token.
 * @return Token z tabulky tokenu.
 */
token_t getTokenAssertArr(int tok_amount, token_type_t *type, int err_code);

/**
 * @brief Vrati ukazatel v tabulce tokenu o 1 zpet.
 *
 */
void ungetToken();

#endif