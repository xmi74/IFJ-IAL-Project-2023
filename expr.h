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

// Prototypy funkcii
int getTokenIndex(token_t token);

bool tokenIsTerm(token_t token);

bool tokenIsOperator(token_t token);

token_type_t getTokenType(token_t * token, local_symtab_w_par_ptr_t *table, global_symtab_t *globalTable);

bool checkOperands(token_t operand1, token_t operand2);

bool dataTypeEqual(token_t token1, token_t token2, token_t operation);

void reduceArithmetic(Stack *stack);

bool reduceLogical(Stack *stack);

void reduceNot(Stack *stack);

void reduceParenthesis(Stack *stack);

void reduceTerm(Stack *stack);

void applyRule(Stack *stack);

bool expressionEnd(token_t *token, token_t prevToken, bool condition);

int getPrecedence(token_t stackTop, token_t token);

ast_node_t * checkExpression(local_symtab_w_par_ptr_t *table, global_symtab_t *globalTable, bool isCondition);

#endif // EXPR_H