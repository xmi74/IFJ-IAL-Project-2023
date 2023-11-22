/**
 * IFJ-IAL Projekt 2023
 *
 * @file stack.h
 * @brief Header file for the implementation of a dynamic ADT stack
 *
 * @author Igor Mikula (xmikul74)
 * @author Marko Olesak (xolesa00)
 * @author Jan Findra (xfindr01)
 * @author Tomas Artl (xarltt00)
 */

#include "stack.h"
#include "token_table.h"
#include "error.h"
#include "abstract_syntax_tree.h"

#ifndef EXPR_H
#define EXPR_H

#define PRETABLESIZE 16

enum
{
    L, // <
    R, // >
    E, // =
    U, // Undefined
};

int precedenceTable[PRETABLESIZE][PRETABLESIZE] = {
    /*! *  /  +  -  == != <  > <=  >= ?? (  )  i  $ */
    {U, R, R, R, R, R, R, R, R, R, R, R, L, R, L, R}, // !
    {L, R, R, R, R, R, R, R, R, R, R, R, L, R, L, R}, // *
    {L, R, R, R, R, R, R, R, R, R, R, R, L, R, L, R}, // /
    {L, L, L, R, R, R, R, R, R, R, R, R, L, R, L, R}, // +
    {L, L, L, R, R, R, R, R, R, R, R, R, L, R, L, R}, // -
    {L, L, L, L, L, U, U, U, U, U, U, R, L, R, L, R}, // ==
    {L, L, L, L, L, U, U, U, U, U, U, R, L, R, L, R}, // !=
    {L, L, L, L, L, U, U, U, U, U, U, R, L, R, L, R}, // <
    {L, L, L, L, L, U, U, U, U, U, U, R, L, R, L, R}, // >
    {L, L, L, L, L, U, U, U, U, U, U, R, L, R, L, R}, // <=
    {L, L, L, L, L, U, U, U, U, U, U, R, L, R, L, R}, // >=
    {L, L, L, L, L, L, L, L, L, L, L, U, L, R, L, R}, // ??
    {L, L, L, L, L, L, L, L, L, L, L, L, L, E, L, U}, // (
    {U, R, R, R, R, R, R, R, R, R, R, R, U, R, U, R}, // )
    {U, R, R, R, R, R, R, R, R, R, R, R, U, R, U, R}, // i
    {L, L, L, L, L, L, L, L, L, L, L, L, L, L, L, E}, // $ (EOF)
};

// Protype function header file
int getTokenIndex(token_t token);

int getPrecedence(int tokenIndex);

bool dataTypeEqual(token_t token1, token_t token2);

bool isIdentifier(token_t token);

void reduceArithmetic(Stack *stack);

bool reduceLogical(Stack *stack, local_symtab_t *table);

bool applyRule(Stack *stack, local_symtab_t *table);

void reduceParenthesis(Stack *stack);

bool checkExpression(local_symtab_t *table);

#endif // EXPR_H