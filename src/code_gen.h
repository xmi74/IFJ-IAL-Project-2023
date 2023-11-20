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

//create new line
string_t *new_line(char *string);

//create append line
void append_line(string_t *str1, char* str2);

// zacanie generovania
string_t *gen_start();

void gen_end(string_t *output);

void gen_value(string_t *output, token_t *token);

void gen_var(string_t *output, token_t *token, bool function);

void gen_func(string_t *output, token_t *token);

void gen_func_end(string_t *output);

void gen_expr(string_t *output, ast_node_t *tree);

void gen_read_str(string_t *output);

void gen_read_int(string_t *output);

void gen_read_doub(string_t *output);

#endif