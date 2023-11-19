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
#include "output.h"

// zacanie generovania
output_t *gen_start();

void gen_end(output_t *output);

void gen_value(output_t *output, token_t *token, bool function);

void gen_func(output_t *output, token_t *token);

void gen_func_end(output_t *output);

#endif