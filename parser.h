/**
 * IFJ-IAL Projekt 2023
 *
 * @file parser.h
 * @brief Implementacia programu syntakticke a semanticke analyzy
 *
 * @author Igor Mikula (xmikul74)
 * @author Marko Olesak (xolesa00)
 * @author Jan Findra (xfindr01)
 * @author Tomas Arlt (xarltt00)
 */

#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>
#include "symtable.h"
#include "expr.h"
#include "code_gen.h"

/**
 * @brief prevede nazev klicoveho slova na datovy typ.
 *
 * @param kw Klicove slovo.
 * @return Datovy typ.
 */
token_type_t kw_to_token_type(token_type_t kw);

/**
 * @brief Funkce pro preskoceni komentaru.
 *
 */
void ignore_comments();

/**
 * @brief Konvertuje typ z type_t na token_type_t.
 *
 * @param type Typ, ktery se ma konvertovat.
 * @return Konvertovany typ.
 */
token_type_t type_t_to_token_type_t(type_t type);

/**
 * @brief Konvertuje typ z token_type_t na type_t.
 *
 * @param type Typ, ktery se ma konvertovat.
 * @return Konvertovany typ.
 */
type_t token_type_t_to_type_t(token_type_t type);

/**
 * @brief Nacte vestavene funkce.
 *
 */
void load_built_in_functions();

/**
 * @brief Zpracovani volani funkce
 *
 * @param func Ukazatel do tabulky symbolu na funkci.
 * @param local_table Ukazatel na lokalni tabulku symbolu.
 * @param global_table Ukazatel na globalni tabulku symbolu.
 * @return Typ navratove hodnoty funkce a informace zda muze obsahovat nil.
 */
token_t call_func(global_symtab_t *func, local_symtab_w_par_ptr_t *local_table, global_symtab_t *global_table);

/**
 * @brief Zpracovani nove promenne.
 *
 * @param token_assigner Token, ktery urcuje, zda se jedna o konstantu nebo promennou.
 * @param global_table Ukazatel na globalni tabulku symbolu.
 * @param local_table Ukazatel na lokalni tabulku symbolu.
 * @param nest_level Uroven zanoreni.
 */
void handle_variable(token_t token_assigner, global_symtab_t **global_table, local_symtab_w_par_ptr_t *local_table, int nest_level);

/**
 * @brief Prirazeni hodnoty do promenne/volani funkce.
 *
 * @param token_id Nazev promenne ci funkce.
 * @param global_table Ukazatel na globalni tabulku symbolu.
 * @param local_table Ukazatel na lokalni tabulku symbolu.
 * @param nest_level Uroven zanoreni.
 */
void handle_assign_or_call_func(token_t token_id, global_symtab_t *global_table, local_symtab_w_par_ptr_t *local_table, int nest_level);

/**
 * @brief Zpracovani definice funkce.
 *
 * @param global_table Ukazatel na globalni tabulku symbolu.
 * @param local_table_one_up Ukazatel na lokalni tabulku symbolu.
 */
void handle_func_def(global_symtab_t *global_table, local_symtab_w_par_ptr_t *local_table_one_up);

/**
 * @brief Zpracovani podminky
 *
 * @param local_table Ukazatel na lokalni tabulku symbolu.
 * @param global_table Ukazatel na globalni tabulku symbolu.
 */
void handle_cond(local_symtab_w_par_ptr_t *local_table, global_symtab_t *global_table);

/**
 * @brief Zpracovani podminky if.
 *
 * @param nest_level Uroven zanoreni.
 * @param local_table Ukazatel na lokalni tabulku symbolu.
 * @param global_table Ukazatel na globalni tabulku symbolu.
 */
void handle_if(int nest_level, local_symtab_w_par_ptr_t *local_table, global_symtab_t *global_table);

/**
 * @brief Zpracovani podminky while.
 *
 * @param nest_level Uroven zanoreni.
 * @param local_table Ukazatel na lokalni tabulku symbolu.
 * @param global_table Ukazatel na globalni tabulku symbolu.
 */
void handle_while(int nest_level, local_symtab_w_par_ptr_t *local_table, global_symtab_t *global_table);

/**
 * @brief Precte podblok, ignoruje obsah.
 *
 * @param token Token, ktery urcuje zda se jedna o blok v {} nebo ().
 */
void read_subblock(token_t token);

/**
 * @brief Najde funkce v kodu, ulozi je to globalni tabulky symbolu.
 *
 * @param global_table Ukazatel na globalni tabulku symbolu.
 */
void find_functions(global_symtab_t **global_table);

/**
 * @brief Zpracovani bloku kodu.
 *
 * @param nest_level Uroven zanoreni.
 * @param block_start Token, ktery urcuje zda se jedna o blok v {} nebo ().
 * @param global_table Ukazatel na globalni tabulku symbolu.
 * @param local_table_one_up Ukazatel na lokalni tabulku symbolu.
 * @param var_name Nazev promenne, ktera se ma na zacatku vlozit do tabulky symbolu.
 * @param var_type Typ promenne, ktera se ma na zacatku vlozit do tabulky symbolu.
 * @param return_type Predpokladany typ navratove hodnoty.
 * @return Udava zda blok obsahuje return.
 */
bool parse_block(int nest_level, token_type_t block_start, global_symtab_t **global_table, local_symtab_w_par_ptr_t *local_table_one_up, string_t *var_name, token_type_t var_type, token_type_t expected_return);

/**
 * @brief Hlavni funkce parseru.
 *
 */
int parse();

#endif // PARSER_H