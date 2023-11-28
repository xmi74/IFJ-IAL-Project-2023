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
#include "code_gen.h"

#ifndef EXPR_H
#define EXPR_H

#define PRETABLESIZE 16

// Protype function header file
token_type_t getTokenType(token_t token, local_symtab_w_par_ptr_t *table, global_symtab_t *globalTable);

bool checkOperands(token_t operand1, token_t operand2);

int getTokenIndex(token_t token);

int getPrecedence(int tokenIndex);

bool dataTypeEqual(token_t token1, token_t token2);

bool isIdentifier(token_t token);

void reduceArithmetic(Stack *stack);

bool reduceLogical(Stack *stack);

void reduceNot(Stack *stack);

void applyRule(Stack *stack);

void reduceParenthesis(Stack *stack);

bool checkExpression(local_symtab_w_par_ptr_t *table, global_symtab_t *globalTable);

#endif // EXPR_H