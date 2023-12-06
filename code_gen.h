/**
 * IFJ-IAL Projekt 2023
 * 
 * @file code_gen.h
 * @brief Generator kodu 3000
 * 
 * @author Igor Mikula (xmikul74)
 * @author Marko Olesak (xolesa00)
 * @author Jan Findra (xfindr01)
 * @author Tomas Arlt (xarltt00)
*/

#ifndef CODE_GEN_H
#define CODE_GEN_H

#include <stdio.h>
#include <stdlib.h>
#include "scanner.h"
#include "abstract_syntax_tree.h"
#include "stack.h"

/**
 * @brief Funkcia na vytvorenie dynamickeho stringu
 * 
 * @param string Staticky string, ktory ma byt vlozeny do dynamickeho stringu
 * @return Ukazatel na dynamicky string
*/
string_t *new_line(char *string);

/**
 * @brief Funkcia na pridanie stringu do dynamickeho stringu
 * 
 * @param str1 Dynamicky string, do ktoreho sa bude pridavat
 * @param str2 Staticky string, ktory sa prida
*/
void append_line(string_t *str1, char* str2);

/**
 * @brief Funkcia na vlozenie pocitadla na vrchol zasobnika
 * 
 * @param counter Cislo, ktore sa ma vlozit
*/
void push_counter(int counter);

/**
 * @brief Funkcia na ziskanie hodnoty na vrchole zasobniku pocitadiel
 * 
 * @return Hodnota pocitadla na vrchole
*/
int top_counter();

/**
 * @brief Funkcie na odstranenie pocitadla z vrcholu zasobnika
*/
void pop_counter();

/**
 * @brief Funkcia na skopirovanie premennych z lokalneho ramca na docasny
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
*/
void newTF(string_t *output);

/**
 * @brief Funkcia na aktualizovanie premennych v nadradenom bloku
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
*/
void updateValues(string_t *output);

/**
 * @brief Funkcia na vytvorenie zaciatku generovaneho outputu
 * 
 * @return Ukazatel na dynamicky string, v ktorom je output
*/
string_t *gen_start();

/**
 * @brief Funkcia na vytvorenie konca generovaneho outputu
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
*/
void gen_end(string_t *output);

/**
 * @brief Funkcia na nacitanie hodnoty
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
 * @param token Token, ktoreho hodnota sa ma nacitat
 * @param isVariable True, ak je to premenna, false inak
 * @param name Nazov premennej
*/
void gen_value(string_t *output, token_t *token, bool isVariable, char* name);

/**
 * @brief Funkcia na vytvorenie premennej
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
 * @param name Nazov premennej
 * @param includesNil True, ak premenna moze obsahovat nil, false inak
 * @param type Typ tokenu
*/
void gen_var(string_t *output, char *name, bool includesNil, token_type_t type);

/**
 * @brief Funckia na priradenie hodnoty premennnej
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
 * @param name Nazov premennej
 * @param type Typ tokenu
*/
void gen_assign(string_t *output, char *name, token_type_t type);

/**
 * @brief Funkcia na vygenerovanie zaciatku definicie funkcie
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
 * @param token Token obsahujuci nazov funkcie
*/
void gen_func(string_t *output, token_t *token);

/**
 * @brief Funckia na vygenerovanie navratu z funkcie
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
*/
void gen_func_return(string_t *output);

/**
 * @brief Funckia na vygenerovanie konca definicie funkcie
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
 * @param token Token obsahujuci nazov funckie
*/
void gen_func_end(string_t *output, token_t *token);

/**
 * @brief Funckia na volanie funkcie
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
 * @param name Nazov volanej funkcie
*/
void gen_func_call(string_t *output, char *name);

/**
 * @brief Funckia na spracovanie expression-u
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
 * @param tree Ukazatel na AST strom obsahujuci spracovany expression
*/
void gen_expr(string_t *output, ast_node_t *tree);

/**
 * @brief Funkcia na generovanie zaciatku if konstrukcie
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
*/
void gen_if(string_t *output);

/**
 * @brief Funkcia na vyhodnotenie podmienky typu if-let
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
 * @param name Nazov skumanej premennej
*/
void gen_if_let(string_t *output, char *name);

/**
 * @brief Funkcia na vygenerovanie else
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
*/
void gen_else(string_t *output);

/**
 * @brief Funkcia na generovania konca if-else konstrukcie
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
*/
void gen_if_end(string_t *output);

/**
 * @brief Funckia na genrovanie zaciatku while konstrukcie
 * @param output Ukazatel na dynamicky string, v ktorom je output
*/
void gen_while(string_t *output);

/**
 * @brief Funkcia na genrovanie tela while konstrukcie
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
*/
void gen_while_body(string_t *output);

/**
 * @brief Funckia na generovanie konca while konstrukcie
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
*/
void gen_while_end(string_t *output);

/**
 * @brief Funkcia na citanie stringu zo stdin
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
*/
void gen_read_str(string_t *output);

/**
 * @brief Funckia na citanie integer-u zo stdin
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
*/
void gen_read_int(string_t *output);

/**
 * @brief Funkcia na citanie double-u zo stdin
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
*/
void gen_read_doub(string_t *output);

/**
 * @brief Funkcia na vytlacenie argumentu/ov na stdout
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
*/
void gen_write(string_t *output);

/**
 * @brief Funkcia na transformaciu int na double
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
*/
void gen_int2double(string_t *output);

/**
 * @brief Funckia na trnasformaciu double na int
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
*/
void gen_double2int(string_t *output);

/**
 * @brief Funkcia na ziskanie dlzky string-u
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
*/
void gen_length(string_t *output);

/**
 * @brief Funkcia na vytvorenie substringu
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
*/
void gen_substring(string_t *output);

/**
 * @brief Funkcia na ziskanie ordinalnej hodnoty prveho znaku v stringu
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
*/
void gen_ord(string_t *output);

/**
 * @brief Funkcia na ziskanie prveho znaku v stringu
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
*/
void gen_chr(string_t *output);

/**
 * @brief Pomocna funkcia na zistenie rovnosti
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
*/
void gen_eq(string_t *output);

/**
 * @brief Pomocna funkcia na zistenie nerovnosti
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
*/
void gen_neq(string_t *output);

/**
 * @brief Pomocna funkcia na zistenie, ci je lavy operand mensi ako pravy
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
*/
void gen_lesser(string_t *output);

/**
 * @brief Poomocna funkcia na zistenie, ci je lavy operand mensi alebo rovny ako pravy
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
*/
void gen_lesser_or_eq(string_t *output);

/**
 * @brief Pomocna funkcia na zistenie, ci je lavy operand vacsi ako pravy
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
*/
void gen_greater(string_t *output);

/**
 * @brief Pomocna funkcia na zistenie, ci je lavy operand vacsi alebo rovny ako pravy
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
*/
void gen_greater_or_eq(string_t *output);

/**
 * @brief Pomocna funkcia na odstranenie nil
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
*/
void gen_questionm(string_t *output);

/**
 * @brief Pomocna funkcia na konkatenaciu
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
*/
void gen_concat(string_t *output);

/**
 * @brief Pomocna funkcia na pripravu hodnot pre dalsie spracovanie
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
*/
void prepare_values(string_t *output);

#endif