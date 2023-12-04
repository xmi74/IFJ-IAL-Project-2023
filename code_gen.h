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

string_t *new_line(char *string);

void append_line(string_t *str1, char* str2);

void push_counter(int counter);

int top_counter();

void pop_counter();

string_t *gen_start();

void gen_end(string_t *output);

void gen_value(string_t *output, token_t *token, bool isVariable, char* name);

void gen_var(string_t *output, char *name, bool includesNil);

void gen_assign(string_t *output, char *name);

void gen_func(string_t *output, token_t *token);

void gen_func_return(string_t *output, token_t *token);

void gen_func_end(string_t *output, token_t *token);

void gen_func_call(string_t *output, char *name);

void gen_expr(string_t *output, ast_node_t *tree);

void gen_if(string_t *output);

void gen_if_let(string_t *output, char *name);

void gen_else(string_t *output);

void gen_if_end(string_t *output);

void gen_while(string_t *output);

void gen_while_body(string_t *output);

void gen_while_end(string_t *output);

void gen_read_str(string_t *output);

void gen_read_int(string_t *output);

void gen_read_doub(string_t *output);

void gen_write(string_t *output);

void gen_int2double(string_t *output);

void gen_double2int(string_t *output);

void gen_length(string_t *output);

void gen_substring(string_t *output);

void gen_ord(string_t *output);

void gen_chr(string_t *output);

void gen_eq(string_t *output);

void gen_neq(string_t *output);

void gen_lesser(string_t *output);

void gen_lesser_or_eq(string_t *output);

void gen_greater(string_t *output);

void gen_greater_or_eq(string_t *output);

void gen_questionm(string_t *output);

void gen_concat(string_t *output);

void prepare_values(string_t *output);

#endif