/**
 * IFJ-IAL Projekt 2023
 * 
 * @file code_gen.c
 * @brief Generator kodu 3000
 * 
 * @author Igor Mikula (xmikul74)
 * @author Marko Olesak (xolesa00)
 * @author Jan Findra (xfindr01)
 * @author Tomas Arlt (xarltt00)
*/

#include "code_gen.h"

output_t gen_start(){
    output_t output;
    output_init(&output);
    output_insert_line(&output, ".IFJcode2023");
    return(output);
}

void gen_var(output_t *output, token_t *token, int nest_level){
    switch (token->type){
        case TOK_KW_VAR: {
            char *prefix = "GF@";
            strcat(prefix, token->attribute.str.data);
            output_insert_line(output, prefix);
        }
    }
}