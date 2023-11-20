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

string_t *new_line(char *string){
    string_t *newLine = malloc(sizeof(string_t));
    dstringInit(newLine);
    int index = 0;
    while (string[index] != '\0'){
        dstringAppend(newLine, string[index++]);
    }
    dstringAppend(newLine, string[index]);
    return newLine;
}

void append_line(string_t *str1, char* str2){
    str1->data[str1->length - 1] = str2[0];
    int index = 1;
    while (str2[index] != '\0'){
        dstringAppend(str1, str2[index++]);
    }
    dstringAppend(str1, str2[index]);
}

string_t *gen_start(){
    string_t *output = new_line(".IFJcode2023\n");
    append_line(output, "JUMP main\n");
    gen_read_str(output);
    gen_read_int(output);
    gen_read_doub(output);
    return(output);
}

void gen_end(string_t *output){
    append_line(output, "EXIT int@\n");
    fprintf(stdout, "%s", output->data);
    dstringFree(output);
}

void gen_value(string_t *output, token_t *token){
    switch (token->type) {
        case TOK_INT: {
            append_line(output, "PUSHS int@");
            char str[16];
            sprintf(str, "%d\n", token->attribute.intValue);
            append_line(output, str);
            break;
        }
        
        case TOK_DOUBLE: {
            append_line(output, "PUSHS float@");
            char str[32];
            sprintf(str, "%a\n", token->attribute.doubleValue);
            append_line(output, str);
            break;
        }

        case TOK_STRING: {
            append_line(output, "PUSHS string@");
            
            for (size_t index = 0; index < token->attribute.str.length; index++){
                char c = token->attribute.str.data[index];
                char str[5];
                if ((c >= 0 && c <= 32) || (c == 35) || (c == 92)){
                    sprintf(str, "%03d", c);
                    append_line(output, "\\");
                    append_line(output, str);
                }
                else{
                    str[0] = c;
                    str[1] = '\0';
                    append_line(output, str);
                }
            }
            append_line(output, "\n");
            break;
        }
        default:
            returnError(INTERN_ERR);
    }
}

void gen_var(string_t *output, token_t *token, bool function){
    append_line(output, "DEFVAR ");
    if (function){
        append_line(output, "LF@");
    }
    else{
        append_line(output, "GF@");
    }

    append_line(output, token->attribute.str.data);
    append_line(output, "\n");
}

void gen_func(string_t *output, token_t *token){
    append_line(output, "LABEL ");
    append_line(output, token->attribute.str.data);
    append_line(output, "\nCREATEFRAME\n"
                        "PUSHFRAME\n"
                        "DEFVAR LF@tmp1\n");
}

void gen_func_end(string_t *output){
    append_line(output, "POPFRAME\n"
                        "RETURN\n");
}

void gen_expres(string_t *output, ast_node_t *tree){
    ast_items_t *items = malloc(sizeof(ast_items_t));
    items_init(items);
    ast_postorder(tree, items);
    int index = 0;
    while (index < items->size){
        switch (items->nodes[index]->token.type){
            case TOK_INT:
            case TOK_DOUBLE:
            case TOK_STRING:{
                gen_value(output, &(items->nodes[index]->token));
                break;
            }
            case TOK_MUL:{
                append_line(output, "MULS\n");
                break;
            }
            case TOK_DIV:{
                append_line(output, "DIVS\n");
                break;
            }
            case TOK_PLUS:{
                append_line(output, "ADDS\n");
                break;
            }
            case TOK_MINUS:{
                append_line(output, "SUBS\n");
                break;
            }
        }
    }
}

void gen_read_str(string_t *output){
    append_line(output, "# buildin functions\n"
                "LABEL readString\n"
                "CREATEFRAME\n"
                "PUSHFRAME\n"
                "LF@str\n"
                "READ LF@str string\n"
                "PUSHS LF@str\n"
                "POPFRAME\n"
                "RETURN\n");
}

void gen_read_int(string_t *output){
    append_line(output, "LABEL readInt\n"
                "CREATEFRAME\n"
                "PUSHFRAME\n"
                "LF@inte\n"
                "READ LF@inte int\n"
                "PUSHS LF@inte\n"
                "POPFRAME\n"
                "RETURN\n");
}

void gen_read_doub(string_t *output){
    append_line(output, "LABEL readDoub\n"
                "CREATEFRAME\n"
                "PUSHFRAME\n"
                "LF@doub\n"
                "READ LF@doub float\n"
                "PUSHS LF@doub\n"
                "POPFRAME\n"
                "RETURN\n");
}