/**
 * IFJ-IAL Projekt 2023
 * 
 * @file symtable.h
 * @brief Implementacia tabulky symbolov (identifikatorov)
 * 
 * @author Igor Mikula (xmikul74)
 * @author Marko Olesak (xolesa00)
 * @author Jan Findra (xfindr01)
 * @author Tomas Arlt (xarltt00)
*/

#ifndef _SYMTABLE_H_
#define _SYMTABLE_H_

#include "scanner.h"
#include <stdbool.h>

typedef enum type_t
{
    T_INT,
    T_DOUBLE,
    T_STRING,
    T_NIL
} type_t;

// Premenna - definovana podla let / var
// Premenna - inicializovana ?

typedef struct local_symtab
{
    string_t key;               
    token_type_t type;          // Typ lokalnej premennej
    int height;                 // Vyska stromu
    int depth;                  // Hlbka zanorenia
    struct local_symtab *left;  // Lavy potomok
    struct local_symtab *right; // Pravy potomok
} local_symtab_t;

typedef struct local_symtab_w_par_ptr
{
    struct local_symtab *table;
    struct local_symtab_w_par_ptr *parent;
} local_symtab_w_par_ptr_t;

typedef struct func_param
{
    token_attribute_t name;
    token_attribute_t identifier;
    token_type_t type;
} func_param_t;

typedef struct global_symtab
{
    string_t key;
    type_t type;                // Typ funkcie/globalnej premennej
    int height;                 // Vyska stromu
    int depth;                  // Hlbka zanorenia
    bool defined;               // Funkcia - deklarovana ci len definovana
    int param_count;            // Pocet parametru funkce
    func_param_t *params;       // Parametre funkcie
    struct global_symtab *left; // Lavy potomok
    struct global_symtab *right;// Pravy potomok
} global_symtab_t;


/****************************************************/
/*         FUNKCIE LOKALNEJ TABULKY SYMBOLOV        */
/****************************************************/

/* VYSKOVE VYVAZENIE LOKALNEJ TABULKY SYMBOLOV */
int max(int n1, int n2);    // Zistenie maxima z dvoch cisel
int heightL(local_symtab_t *ptr);    // Zistenie vysky stromu
int balanceL(local_symtab_t *local_symtab);  // Urcuje hodnotu vyvazenosti
local_symtab_t* rotateRightL(local_symtab_t *criticalNode);  // Rotacia lokalnej tabulky doprava
local_symtab_t* rotateLeftL(local_symtab_t *criticalNode);   // Rotacia lokalnej tabulky dolava

/* FUNKCIE LOKALNEJ TABULKY SYMBOLOV */

// Inicializacia lokalnej tabulky
void local_init(local_symtab_t **local_table);

// Inicializace lokalni tabulky s ukazatelem na predka
void local_init_w_par_ptr_t(local_symtab_w_par_ptr_t *local_table);

// Vyhladanie symbolu (na zaklade kluca - key) v lokalnej tabulke
local_symtab_t *local_search(local_symtab_t *local_table, string_t *key);

// Vyhledavani symbolu v lokalni tabulce a v jejich predcich
local_symtab_t *local_search_in_all(local_symtab_w_par_ptr_t *local_table, string_t *key);

// Vlozenie symbolu do lokalnej tabulky
local_symtab_t* local_insert(local_symtab_t *local_table, string_t *key, token_type_t type);

// Uvolnenie pamati lokalnej tabulky
void local_dispose(local_symtab_t **local_table);


/****************************************************/
/*         FUNKCIE GLOBALNEJ TABULKY SYMBOLOV       */
/****************************************************/

/* VYSKOVE VYVAZENIE GLOBALNEJ TABULKY SYMBOLOV */
// Funkcnost analogicky k lokalnej tabulke
int heightG(global_symtab_t *ptr);    
int balanceG(global_symtab_t *local_symtab);  
global_symtab_t* rotateRightG(global_symtab_t *criticalNode);  
global_symtab_t* rotateLeftG(global_symtab_t *criticalNode);   

// Inicializacia globalnej tabulky
void global_init(global_symtab_t **global_table);

// Vyhladanie symbolu (na zaklade kluca - key) v globalnej tabulke
global_symtab_t *global_search(global_symtab_t *global_table, string_t *key);

// Vlozenie symbolu do globalnej tabulky
global_symtab_t* global_insert(global_symtab_t *global_table, string_t *key, type_t type, int param_count, func_param_t *params);

// Uvolnenie pamati globalnej tabulky
void global_dispose(global_symtab_t **global_table);

#endif
