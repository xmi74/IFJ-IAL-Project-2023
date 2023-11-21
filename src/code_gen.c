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
    string_t *output = new_line(".IFJcode23\n");
    append_line(output, "DEFVAR GF@tmp0\n"
                        "DEFVAR GF@tmp1\n"
                        "DEFVAR GF@tmp2\n"
                        "DEFVAR GF@tmp3\n"
                        "DEFVAR GF@type0\n"
                        "DEFVAR GF@type1\n");
    append_line(output, "JUMP main\n");
    gen_read_str(output);
    gen_read_int(output);
    gen_read_doub(output);
    gen_eq(output);
    gen_neq(output);
    gen_lesser(output);
    gen_lesser_or_eq(output);
    gen_greater(output);
    gen_greater_or_eq(output);
    gen_questionm(output);
    gen_concat(output);
    // remove than
    append_line(output, "LABEL main\n");
    return(output);
}

void gen_end(string_t *output){
    append_line(output, "EXIT int@0\n");
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

void gen_expr(string_t *output, ast_node_t *tree){
    ast_items_t *items = malloc(sizeof(ast_items_t));
    items_init(items);
    ast_postorder(tree, items);
    int index = 0;
    bool isString = false;
    while (index < items->size){
        switch (items->nodes[index]->token.type){
            case TOK_INT:
            case TOK_DOUBLE:{
                isString = false;
                gen_value(output, &(items->nodes[index]->token));
                break;
            }
            case TOK_STRING:{
                isString = true;
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
                if (isString){
                    append_line(output, "CALL concat\n");
                }
                else{
                    append_line(output, "ADDS\n");
                }
                break;
            }
            case TOK_MINUS:{
                append_line(output, "SUBS\n");
                break;
            }
            case TOK_ASSIGN:{
                append_line(output, "CALL equals\n");
                break;
            }
            case TOK_NOT_EQUAL:{
                append_line(output, "CALL nequals\n");
                break;
            }
            case TOK_LESSER:{
                append_line(output, "CALL lesser\n");
                break;
            }
            case TOK_LESSER_OR_EQUAL:{
                append_line(output, "CALL lesser_or_equal\n");
                break;
            }
            case TOK_GREATER:{
                append_line(output, "CALL greater\n");
                break;
            }
            case TOK_GREATER_OR_EQUAL:{
                append_line(output, "CALL greater_or_equal\n");
                break;
            }
            case TOK_DOUBLE_QUEST_MARK:{
                append_line(output, "CALL remove_nil\n");
                break;
            }
        }
        index++;
    }
}

// builtin functions

void gen_read_str(string_t *output){
    append_line(output, "# read string\n"
                "LABEL readString\n"
                "CREATEFRAME\n"
                "PUSHFRAME\n"
                "DEFVAR LF@str\n"
                "READ LF@str string\n"
                "PUSHS LF@str\n"
                "POPFRAME\n"
                "RETURN\n");
}

void gen_read_int(string_t *output){
    append_line(output, "# read integer\n"
                "LABEL readInt\n"
                "CREATEFRAME\n"
                "PUSHFRAME\n"
                "DEFVAR LF@inte\n"
                "READ LF@inte int\n"
                "PUSHS LF@inte\n"
                "POPFRAME\n"
                "RETURN\n");
}

void gen_read_doub(string_t *output){
    append_line(output, "# read float\n"
                "LABEL readDoub\n"
                "CREATEFRAME\n"
                "PUSHFRAME\n"
                "DEFVAR LF@doub\n"
                "READ LF@doub float\n"
                "PUSHS LF@doub\n"
                "POPFRAME\n"
                "RETURN\n");
}

// pomocne funkcie

void gen_eq(string_t *output){
    append_line(output, "# function equal\n"
                        "LABEL equals\n"
                        "POPS GF@tmp0\n"
                        "POPS GF@tmp1\n"
                        "JUMPIFEQ eq_true GF@tmp0 GF@tmp1\n"
                        "PUSHS bool@false\n"
                        "JUMP eq_end\n"
                        "LABEL eq_true\n"
                        "PUSHS bool@true\n"
                        "LABEL eq_end\n"
                        "RETURN\n");
}

void gen_neq(string_t *output){
    append_line(output, "# function not equal\n"
                        "LABEL nequals\n"
                        "POPS GF@tmp0\n"
                        "POPS GF@tmp1\n"
                        "JUMPIFEQ neq_true GF@tmp0 GF@tmp1\n"
                        "PUSHS bool@true\n"
                        "JUMP neq_end\n"
                        "LABEL neq_true\n"
                        "PUSHS bool@false\n"
                        "LABEL neq_end\n"
                        "RETURN\n");
}

void gen_lesser(string_t *output){
    append_line(output, "# function lesser\n"
                        "LABEL lesser\n"
                        "POPS GF@tmp0\n"
                        "POPS GF@tmp1\n"
                        "LT GF@tmp2 GF@tmp0 GF@tmp1\n"
                        "PUSHS GF@tmp2\n"
                        "RETURN\n");
}

void gen_lesser_or_eq(string_t *output){
    append_line(output, "# function lesser or equal\n"
                        "LABEL lesser_or_equal\n"
                        "POPS GF@tmp0\n"
                        "POPS GF@tmp1\n"
                        "LT GF@tmp2 GF@tmp0 GF@tmp1\n"
                        "EQ GF@tmp3 GF@tmp0 GF@tmp1\n"
                        "PUSHS GF@tmp2\n"
                        "PUSHS GF@tmp3\n"
                        "ORS\n"
                        "RETURN\n");
}

void gen_greater(string_t *output){
    append_line(output, "# function greater\n"
                        "LABEL greater\n"
                        "POPS GF@tmp0\n"
                        "POPS GF@tmp1\n"
                        "PUSHS GF@tmp1\n"
                        "PUSHS GF@tmp0\n"
                        "CALL lesser\n"
                        "RETURN\n");
}

void gen_greater_or_eq(string_t *output){
    append_line(output, "# function greater or equal\n"
                        "LABEL greater_or_equal\n"
                        "POPS GF@tmp0\n"
                        "POPS GF@tmp1\n"
                        "PUSHS GF@tmp1\n"
                        "PUSHS GF@tmp0\n"
                        "CALL lesser_or_equal\n"
                        "RETURN\n");
}

void gen_concat(string_t *output){
    append_line(output, "# concatenation\n"
                        "LABEL concat\n"
                        "POPS GF@tmp0\n"
                        "POPS GF@tmp1\n"
                        "CONCAT GF@tmp2 GF@tmp0 GF@tmp1\n"
                        "PUSHS GF@tmp2\n"
                        "RETURN\n");
}

void gen_questionm(string_t *output){
    append_line(output, "# function remove nil\n"
                        "LABEL remove_nil\n"
                        "POPS GF@tmp0\n"
                        "POPS GF@tmp1\n"
                        "JUMPIFEQ first_nil GF@tmp0 nil@nil\n"
                        "PUSHS GF@tmp0\n"
                        "RETURN\n"
                        "LABEL first_nil\n"
                        "TYPE GF@type0 GF@tmp0\n"
                        "TYPE GF@type1 GF@tmp1\n"
                        "JUMPIFEQ nil_same GF@type0 GF@type1\n"
                        "JUMP error\n" // kde je error?
                        "LABEL nil_same\n"
                        "PUSHS GF@tmp1\n"
                        "RETURN\n");
}

// not sure if necessary
void prepare_values(string_t *output){
    append_line(output, "# prepare values\n"
                        "LABEL prepare\n"
                        "POPS GF@tmp0\n"
                        "TYPE GF@type0\n"
                        "JUMPIFEQ prep_string GF@type0 string@string\n"
                        "JUMPIFEQ prep_float GF@type0 string@string\n");
}