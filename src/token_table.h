/**
 * IFJ-IAL Projekt 2023
 * 
 * @file token_table.h
 * @brief Implementacia programu syntakticke a semanticke analyzy
 * 
 * @author Igor Mikula (xmikul74)
 * @author Marko Olesak (xolesa00)
 * @author Jan Findra (xfindr01)
 * @author Tomas Arlt (xarltt00)
*/

#include "symtable.h"
#include <stdbool.h>

// Struktura tabulky tokenu
typedef struct
{
    token_t *tokens;
    int size;
    int capacity;
    int index;
    bool insert; // vkladani/cteni
} token_table_t;

// Inicializace tabulky tokenu
void initTokenTable(token_table_t *table);

// Uvolneni pameti tabulky tokenu
void freeTokenTable(token_table_t *table);

// Pridani tokenu do tabulky tokenu
void addTableToken(token_table_t *table, token_t token);

// Vraceni tokenu z tabulky tokenu
token_t getTableToken(token_table_t *table);

token_t getToken();

token_t getTokenAssert(token_type_t type);