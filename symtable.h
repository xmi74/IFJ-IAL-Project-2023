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

#ifndef SYMTABLE_H
#define SYMTABLE_H

#include "scanner.h"
#include <stdbool.h>

typedef enum type_t
{
    T_INT,
    T_DOUBLE,
    T_STRING,
    T_NIL
} type_t;

typedef struct local_symtab
{
    string_t key;               
    token_type_t type;          // Typ lokalnej premennej
    bool includesNil;           // Obsahuje nil?
    bool isConstant;            // Je konstanta?
    bool isInitialised;         // Je inicializovana?
    int height;                 // Vyska stromu
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
    // TODO: toto by slo nahradit 1 string_t a token
    string_t name;
    string_t identifier;
    token_type_t type;
    bool includesNil;
} func_param_t;

typedef struct global_symtab
{
    string_t key;
    type_t type;                // Typ funkcie/globalnej premennej
    bool includesNil;           // Obsahuje nil?
    bool isConstant;            // Je konstanta?
    bool isInitialised;         // Je inicializovana?
    int height;                 // Vyska stromu
    bool is_func;               // Je funkce?
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

/**
 * @brief Inicializacia lokalnej tabulky
 * 
 * @param local_table inicializovana struktura
*/
void local_init(local_symtab_t **local_table);

/**
 * @brief Inicializace lokalni tabulky s ukazatelem na predka
 * 
 * @param local_table inicializovana struktura
*/
void local_init_w_par_ptr_t(local_symtab_w_par_ptr_t *local_table);

/**
 * @brief Vyhladanie symbolu (na zaklade kluca - key) v lokalnej tabulke
 * 
 * @param local_table prehladavana struktura
 * @param key hladana polozka
 * 
 * @return odkaz na hladany prvok v strukture, NULL ak prvok nie je pritomny
*/
local_symtab_t *local_search(local_symtab_t *local_table, string_t *key);

/**
 * @brief Vyhledavani symbolu v lokalni tabulce a v jejich predcich
 * 
 * @param local_table prehladavana struktura
 * @param key hladana polozka
 * 
 * @return odkaz na hladany prvok v strukture, NULL ak prvok nie je pritomny
*/
local_symtab_t *local_search_in_all(local_symtab_w_par_ptr_t *local_table, string_t *key);

/**
 * @brief Vlozenie symbolu do lokalnej tabulky
 * 
 * @param local_table struktura, do ktorej sa bude vkladat polozka
 * @param key nazov vkladanej polozky
 * @param type typ vkladanej polozky
 * @param includesNil vkladana polozka je typu, ktory moze zahrnovat nil (true) alebo nemoze zahrnovat nil (false)
 * @param isConstant (true) ak bola premenna definovana pomocou kw let, (false) ak bola premenna definovana pomocou kw var
 * @param isInitialised oznacuje ci polozka uz bola inicializovana (true) alebo nie (false)
 *
 * @return ukazatel na strukturu lokalnej tabulky symbolov s pridanym pozadovanym prvkom
*/
local_symtab_t* local_insert(local_symtab_t *local_table, string_t *key, token_type_t type, bool includesNil, bool isConstant, bool isInitialised);

/**
 * @brief Uvolnenie pamati lokalnej tabulky
 * 
 * @param local_table struktura, ktorej pamat sa ma uvolnit
*/
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

/**
 * @brief Inicializacia struktury globalnej tabulky symbolov
 * 
 * @param global_table inicializovana tabulka
*/
void global_init(global_symtab_t **global_table);

/**
 * @brief Vyhladanie symbolu (na zaklade kluca - key) v globalnej tabulke symbolov
 * 
 * @param global_table prehladavana struktura
 * @param key hladana polozka
 * 
 * @return odkaz na hladany prvok v strukture, NULL ak prvok nie je pritomny
*/
global_symtab_t *global_search(global_symtab_t *global_table, string_t *key);

/**
 * @brief Vlozenie symbolu do globalnej tabulky symbolov
 * 
 * @param global_table struktura, do ktorej sa bude vkladat polozka
 * @param key nazov vkladanej polozky
 * @param type typ vkladanej polozky
 * @param is_func oznacuje ci je vkladana polozka funkcia (true) alebo premenna (false)
 * @param param_count pocet parametrov v pripade funkcie
 * @param params struktura s informaciami o vkladanych parametroch v pripade funkcie
 * @param includesNil vkladana polozka je typu, ktory moze zahrnovat nil (true) alebo nemoze zahrnovat nil (false)
 * @param isConstant (true) ak bola premenna definovana pomocou kw let, (false) ak bola premenna definovana pomocou kw var
 * @param isInitialised oznacuje ci polozka uz bola inicializovana (true) alebo nie (false)
 * 
 * @return ukazatel na strukturu globalnej tabulky symbolov s pridanym pozadovanym prvkom
*/
global_symtab_t* global_insert(global_symtab_t *global_table, string_t *key, type_t type, bool is_func, int param_count, func_param_t *params, bool includesNil, bool isConstant, bool isInitialised);

/**
 * @brief Uvolnenie pamati globalnej tabulky
 * 
 * @param global_table struktura, ktorej pamat sa ma uvolnit
*/
void global_dispose(global_symtab_t **global_table);

#endif
