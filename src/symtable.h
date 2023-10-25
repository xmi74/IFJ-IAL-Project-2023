/**
 * IFJ-IAL Projekt 2023
 * 
 * @file symtable.h
 * @brief Implementacia tabulky symbolov (identifikatorov)
 * 
 * @author Igor Mikula (xmikul74)
 * @author Marko Olesak (xolesa00)
 * @author Jan Findra (xfindr01)
 * @author
*/

#ifndef _SYMTABLE_H_
#define _SYMTABLE_H_

#include "scanner.h"

typedef enum type_t
{
    STRING_T,
    INT_T,
    FLOAT_T,
    NIL_T,
    VOID_T
} type_t;


typedef struct local_symtab
{
    string_t key;
    token_type_t type;
    struct local_symtab *left;
    struct local_symtab *right;
} local_symtab_t;

typedef struct local_symtab_w_par_ptr
{
    struct local_symtab *table;
    struct local_symtab_w_par_ptr *parent;
} local_symtab_w_par_ptr_t;

typedef struct global_symtab
{
    string_t key;
    type_t type;
    string_t params;
    struct global_symtab *left;
    struct global_symtab *right;
} global_symtab_t;


/* FUNKCIE PRE LOKALNU TABULKU SYMBOLOV */

// Inicializacia lokalnej tabulky
void local_init(local_symtab_t **local_table);

// Inicializace lokalni tabulky s ukazatelem na predka
void local_init_w_par_ptr_t(local_symtab_w_par_ptr_t *local_table);

// Vyhladanie symbolu (na zaklade kluca - key) v lokalnej tabulke
local_symtab_t *local_search(local_symtab_t *local_table, string_t *key);

// Vyhledavani symbolu v lokalni tabulce a v jejich predcich
local_symtab_t *local_search_in_all(local_symtab_w_par_ptr_t *local_table, string_t *key);

// Vlozenie symbolu do lokalnej tabulky
void local_insert(local_symtab_t **local_table, string_t *key, token_type_t type);

// Uvolnenie pamati lokalnej tabulky
void local_dispose(local_symtab_t **local_table);


/* FUNKCIE PRE GLOBALNU TABULKU SYMBOLOV */

// Inicializacia globalnej tabulky
void global_init(global_symtab_t **global_table);

// Vyhladanie symbolu (na zaklade kluca - key) v globalnej tabulke
global_symtab_t *global_search(global_symtab_t *global_table, string_t *key);

// Vlozenie symbolu do globalnej tabulky
void global_insert(global_symtab_t **global_table, string_t *key, type_t type, string_t params);

// Uvolnenie pamati globalnej tabulky
void global_dispose(global_symtab_t **global_table);

#endif