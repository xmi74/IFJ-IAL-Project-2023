/**
 * IFJ-IAL Projekt 2023
 *
 * @file expr.h
 * @brief Precedencna analyza vyrazov
 *
 * @author Igor Mikula (xmikul74)
 * @author Marko Olesak (xolesa00)
 * @author Jan Findra (xfindr01)
 * @author Tomas Arlt (xarltt00)
 */


#ifndef EXPR_H
#define EXPR_H

#include "stack.h"
#include "token_table.h"
#include "abstract_syntax_tree.h"
#include "code_gen.h"
#include "error.h"

#define PRETABLESIZE 16

// Prototypy funkcii
/**
 * @brief Pomocna funkcia pre zistenie indexu tokenu v precedencnej tabulke.
 *
 * @param token Token, ktoreho index sa ma zistit.
 * @return Index tokenu v precedencnej tabulke.
 */
int getTokenIndex(token_t token);


/**
 * @brief pomocna funckia pre zistenie ci je token term
 * Term je libovolny literal (celociselny, desatinny, retezcovy ci nil) alebo identifikator.
 * @param token token ktory sa ma skontrolovat
 * @return true ak je term, inak false
 */
bool tokenIsTerm(token_t token);


/**
 * @brief pomocna funckia pre zistenie ci je token operator
 * @param token token ktory sa ma skontrolovat
 * @return true ak je operator, inak false
 */
bool tokenIsOperator(token_t token);

/**
 * @brief pomocna funckia pre zistenie ci je token relational operator
 * @param token token ktory sa ma skontrolovat
 * @return true ak je relational operator, inak false
 */
bool tokenIsRelationalOperator(token_t token);

/**
 * @brief Funkcia na zistenie typu tokenu z tabulky symbolov
 * Funcia tiez nastavuje atribut includesNil
 * @param token token ktoreho typ sa ma zistit
 * @param table tabulka symbolov
 * @return typ tokenu
 */
token_type_t getTokenType(token_t * token, local_symtab_w_par_ptr_t *table, global_symtab_t *globalTable);

/**
 * @brief Funkcia na kontrolu operandov
 * Funkcia tiez kontroluje ci operandy neobsahuju nil, v takom pripade sa jedna o error
 * @param operand1 prvy operand
 * @param operand2 druhy operand
 * @return true ak su operandy v poriadku, inak false
 */
bool checkOperands(token_t operand1, token_t operand2);

/**
 * @brief Funkcia vyhodnocuje kompatibilitu operandov vo vyraze
 * Funkcia zvlast kontroluje operator '??', ktory vyzaaduje specialnu kontrolu
 * @param operand1 Prvy token.
 * @param operand2 Druhy token.
 * @return True ak su datove typy rovnake, inak false.
 */
bool dataTypeEqual(token_t token1, token_t token2, token_t operation);

/**
 * @brief Funkcia na redukciu aritmetickych vyrazov
 * Funkcia tiez tvori uzle stromu
 * @param stack zasobnik
 * @return void
 */
void reduceArithmetic(Stack *stack);

/**
 * @brief Funkcia na redukciu logickych vyrazov
 * Funckia tiez tvori uzle stromu
 * @param stack zasobnik
 * @param table tabulka symbolov
 * @return true ak sa podarila redukcia, resp. datove typy sa rovnaju, inak false
 */
bool reduceLogical(Stack *stack);

/**
 * @brief Funkcia na redukciu NOT operatora
 * @param stack zasobnik
 * @return void
 */
void reduceNot(Stack *stack);

/**
 * @brief Funkcia na redukciu zatvoriek
 * @param stack zasobnik
 * @return void
 */
void reduceParenthesis(Stack *stack);

/**
 * @brief Funkcia na redukciu termu
 * @param stack zasobnik
 * @return void
 */
void reduceTerm(Stack *stack);

/**
 * @brief Funkcia na aplikaciu pravidiel
 * Aplikacia pravidiel podla operatorov
 * @param stack zasobnik
 * @param table tabulka symbolov
 * @return true ak sa aplikacia pravidla podarila, ak pravidlo neexistuje alebo nastala chyba pri redukcii, tak false
 */
void applyRule(Stack *stack);

/**
 * @brief Funkcia na zistenie ci je koniec vyrazu
 * @param token aktualny token
 * @param prevToken predchadzajuci token
 * @return true ak je koniec vyrazu, inak false
 */
bool expressionEnd(token_t *token, token_t prevToken, bool condition);

/**
 * @brief Funkcia na zistenie precedencie
 * @param stackTop token na vrchole zasobnika
 * @param token aktualny token
 * @return int cislo reprezentujuce vyslednu operaciu
 */
int getPrecedence(token_t stackTop, token_t token);

/**
 * @brief hlavna funkcia na analyzu vyrazov
 * Funkcia tiez vola generator kodu, pre vygenerovanie kodu vyrazu z jeho AST stromu.
 * Funkcia urcuje operaciu podla precedencnej tabulky, resp. ci sa ma vykonat LOAD, REDUCE a pod.
 * @param table tabulka symbolov
 * @param globalTable globalna tabulka symbolov
 * @return ast_node_t* ukazatel na koren AST stromu, kde sa nachadzaju potrebne informacie o vyraze
 */
ast_node_t * checkExpression(local_symtab_w_par_ptr_t *table, global_symtab_t *globalTable, bool isCondition);

#endif // EXPR_H