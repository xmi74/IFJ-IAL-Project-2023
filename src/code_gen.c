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

output_t *gen_start(){
    output_t *output;
    output_init(&output);
    string_t *line = new_line(".IFJcode2023");
    output_insert_line(output, line);
    string_t *line2 = new_line("JUMP main");
    output_insert_line(output, line2);
    return(output);
}

void gen_end(output_t *output){
    string_t *line = new_line("EXIT int@1");
    output_insert_line(output, line);
    output_print(output);
}

void gen_value(output_t *output, token_t *token, bool function){
    string_t *line0;
    switch (token->type) {
        case TOK_INT: {
            line0 = new_line("PUSHS int@");
            char str[16];
            sprintf(str, "%d", token->attribute.intValue);
            append_line(line0, str);
            break;
        }
        case TOK_DOUBLE: {
            line0 = new_line("PUSHS float@");
            char str[32];
            sprintf(str, "%a", token->attribute.doubleValue);
            append_line(line0, str);
            break;
        }
        default:
            returnError(INTERN_ERR);
    }
    output_insert_line(output, line0);
}

void gen_func(output_t *output, token_t *token){
    string_t *line0 = new_line("LABEL ");
    append_line(line0, token->attribute.str.data);
    output_insert_line(output, line0);
    string_t *line1 = new_line("CREATEFRAME");
    output_insert_line(output, line1);
    string_t *line2 = new_line("PUSHFRAME");
    output_insert_line(output, line2);
    string_t *line3 = new_line("DEFVAR LF@tmp1");
    output_insert_line(output, line3);
}

void gen_func_end(output_t *output){
    string_t *line0 = new_line("POPFRAME");
    output_insert_line(output, line0);
    string_t *line1 = new_line("RETURN");
    output_insert_line(output, line1);

}