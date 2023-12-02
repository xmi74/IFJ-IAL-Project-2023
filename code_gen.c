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
extern int nestLevel;
extern string_t *localVariables;

/**
 * @brief Funkcia na vytvorenie dynamickeho stringu
 * 
 * @param string Staticky string, ktory ma byt vlozeny do dynamickeho stringu
 * @return Ukazatel na dynamicky string
*/
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

/**
 * @brief Funkcia na pridanie stringu do dynamickeho stringu
 * 
 * @param str1 Dynamicky string, do ktoreho sa bude pridavat
 * @param str2 Staticky string, ktory sa prida
*/
void append_line(string_t *str1, char* str2){
    str1->data[str1->length - 1] = str2[0];
    int index = 1;
    while (str2[index] != '\0'){
        dstringAppend(str1, str2[index++]);
    }
    dstringAppend(str1, str2[index]);
}

/**
 * @brief Funkcia na vytvorenie zaciatku generovaneho outputu
 * 
 * @return Ukazatel na dynamicky string, v ktorom je output
*/
string_t *gen_start(){
    string_t *output = new_line(".IFJcode23\n");
    append_line(output, "DEFVAR GF@tmp_res\n");
    append_line(output, "JUMP main\n");
    // definitions of builtin functions
    gen_read_str(output);
    gen_read_int(output);
    gen_read_doub(output);
    gen_write(output);
    gen_int2double(output);
    gen_double2int(output);
    gen_length(output);
    gen_substring(output);
    gen_ord(output);
    gen_chr(output);
    // definitons of auxilliary functions
    gen_eq(output);
    gen_neq(output);
    gen_lesser(output);
    gen_lesser_or_eq(output);
    gen_greater(output);
    gen_greater_or_eq(output);
    gen_questionm(output);
    gen_concat(output);
    append_line(output, "LABEL main\n");
    return(output);
}

/**
 * @brief Funkcia na vytvorenie konca generovaneho outputu
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
*/
void gen_end(string_t *output){
    append_line(output, "EXIT int@0\n");
    append_line(output, "LABEL error\n"
                        "EXIT int@50\n");
    fprintf(stdout, "%s", output->data);
    dstringFree(output);
}

/**
 * @brief Funkcia na nacitanie hodnoty
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
 * @param token Token, ktoreho hodnota sa ma nacitat
*/
void gen_value(string_t *output, token_t *token, bool isVariable, char* name){
    if (isVariable){
        if (nestLevel == 0){
            append_line(output, "PUSHS GF@");
        }
        else{
            append_line(output, "PUSHS LF@");
        }
        append_line(output, name);
        append_line(output, "\n");
    }
    else{
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
                        append_line(output, "\\\\");
                        sprintf(str, "%03d", c);
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
}

/**
 * @brief Funkcia na vytvorenie premennej
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
 * @param token Token, ktoreho meno udava nazov premennej
 * @param function Bool, ktory udava, ci premmenna je vo funkcii (true - vo funkcii; false - v maine)
*/
void gen_var(string_t *output, token_t *token){
    if (nestLevel == 0){
        append_line(output, "DEFVAR GF@");
        append_line(output, token->attribute.str.data);
        append_line(output, "\n");
    }
    else{
        append_line(output, "DEFVAR LF@");
        append_line(output, token->attribute.str.data);
        append_line(output, "\n");
        char *isDefined = strstr(localVariables->data, token->attribute.str.data);
        if (isDefined == NULL){
            append_line(output, "DEFVAR TF@");
            append_line(output, token->attribute.str.data);
            append_line(output, "\n");
            append_line(localVariables, "DEFVAR TF@");
            append_line(localVariables, token->attribute.str.data);
            append_line(localVariables, "\n");
        }
        append_line(localVariables, "MOVE TF@");
        append_line(localVariables, token->attribute.str.data);
        append_line(localVariables, " LF@");
        append_line(localVariables, token->attribute.str.data);
        append_line(localVariables, "\n");
    }

    append_line(output, "PUSHS nil@nil\n");
    gen_assign(output, token);
}

/**
 * @brief Funckia na priradenie hodnoty premennnej
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
 * @param token Token, ktoreho meno udava nazov premennej
 * @param function Bool, ktory udava, ci premmenna je vo funkcii (true - vo funkcii; false - v maine)
*/
void gen_assign(string_t *output, token_t *token){
    if (nestLevel == 0){
        append_line(output, "POPS GF@");
        append_line(output, token->attribute.str.data);
        append_line(output, "\n");
    }
    else{
        append_line(output, "POPS LF@");
        append_line(output, token->attribute.str.data);
        append_line(output, "\n");
        append_line(output, "MOVE TF@");
        append_line(output, token->attribute.str.data);
        append_line(output, " LF@");
        append_line(output, token->attribute.str.data);
        append_line(output, "\n");
    }
}

/**
 * @brief Funkcia na vygenerovanie zaciatku definicie funkcie
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
 * @param token Token obsahujuci nazov funkcie
*/
void gen_func(string_t *output, token_t *token){
    nestLevel++;
    append_line(output, "JUMP ");
    append_line(output, token->attribute.str.data);
    append_line(output, "_end\n");
    append_line(output, "LABEL ");
    append_line(output, token->attribute.str.data);
    if (nestLevel == 1){
      append_line(output, "\nCREATEFRAME\n"
                            "PUSHFRAME\n");
      localVariables = new_line("CREATEFRAME\n");
    }
    else{
        append_line(output, "\nPUSHFRAME\n");
        append_line(output, localVariables->data);
    }
}

/**
 * @brief Funckia na vygenerovanie konca definicie funkcie
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
 * @param token Token obsahujuci nazov funckie
*/
void gen_func_end(string_t *output, token_t *token){
    nestLevel--;
    if (nestLevel == 0){
        dstringFree(localVariables);
    }
    append_line(output, "POPFRAME\n"
                        "RETURN\n"
                        "LABEL ");
    append_line(output, token->attribute.str.data);
    append_line(output, "_end\n");
}

void gen_func_return(string_t *output, token_t *token){

}

/**
 * @brief Funckia na volanie funkcie
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
 * @param name Nazov volanej funkcie
*/
void gen_func_call(string_t *output, char *name){
    append_line(output, "CALL ");
    append_line(output, name);
    append_line(output, "\n");
}

/**
 * @brief Funckia na spracovanie expression-u
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
 * @param tree Ukazatel na AST strom obsahujuci spracovany expression
*/
void gen_expr(string_t *output, ast_node_t *tree){
    ast_items_t *items = malloc(sizeof(ast_items_t));
    items_init(items);
    ast_postorder(tree, items);
    int index = 0;
    bool isString = false;
    while (index < items->size){
        switch (items->nodes[index]->token.type){
            case TOK_IDENTIFIER:{
                gen_value(output, NULL, true, items->nodes[index]->token.attribute.str.data);
                break;
            }
            case TOK_INT:
            case TOK_DOUBLE:{
                isString = false;
                gen_value(output, &(items->nodes[index]->token), false, NULL);
                break;
            }
            case TOK_STRING:{
                isString = true;
                gen_value(output, &(items->nodes[index]->token), false, NULL);
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
            case TOK_EQUAL:{
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
            default:{
                ;
            }
        }
        index++;
    }
}

/**
 * @brief Funkcia na genrocanie zaciatku if konstrukcie
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
 * @param counter Pocitadlo konstrukcii
*/
void gen_if(string_t *output, int counter){
    nestLevel++;
    append_line(output, "# if condition\n"
                        "POPS GF@tmp_res\n"
                        "JUMPIFNEQ else");
    char str[16];
    sprintf(str, "%d", counter);
    append_line(output, str);
    append_line(output, " GF@tmp_res bool@true\n");
    append_line(output, "# body of if\n");
    if (nestLevel == 1){
      append_line(output, "CREATEFRAME\n"
                            "PUSHFRAME\n");
      localVariables = new_line("CREATEFRAME\n");
    }
    else{
        append_line(output, "\nPUSHFRAME\n");
        append_line(output, localVariables->data);
    }
}

/**
 * @brief Funkcia na vygenerovanie else
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
 * @param counter Pocitadlo konstrukcii
*/
void gen_else(string_t *output, int counter){
    append_line(output, "JUMP if_end");
    char str[16];
    sprintf(str, "%d\n", counter);
    append_line(output, str);
    append_line(output, "# body of else\n"
                        "LABEL else");
    sprintf(str, "%d\n", counter);
    append_line(output, str);
    if (nestLevel == 1){
      append_line(output, "CREATEFRAME\n"
                            "PUSHFRAME\n");
      localVariables = new_line("CREATEFRAME\n");
    }
    else{
        append_line(output, "\nPUSHFRAME\n");
        append_line(output, localVariables->data);
    }
}

/**
 * @brief Funkcia na generovania konca if-else konstrukcie
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
 * @param counter Pocitadlo konstrukcii
*/
void gen_if_end(string_t *output, int counter){
    nestLevel--;
    if (nestLevel == 0){
        dstringFree(localVariables);
    }
    append_line(output, "LABEL if_end");
    char str[16];
    sprintf(str, "%d\n", counter);
    append_line(output, str);
    append_line(output, "POPFRAME\n");
}

/**
 * @brief Funckia na genrovanie zaciatku while konstrukcie
 * @param output Ukazatel na dynamicky string, v ktorom je output
 * @param counter Pocitadlo konstrukcii
*/
void gen_while(string_t *output, int counter){
    nestLevel++;
    if (nestLevel == 1){
      append_line(output, "\nCREATEFRAME\n"
                            "PUSHFRAME\n");
      localVariables = new_line("CREATEFRAME\n");
    }
    else{
        append_line(output, "\nPUSHFRAME\n");
        append_line(output, localVariables->data);
    }
    append_line(output, "LABEL while");
    char str[16];
    sprintf(str, "%d\n", counter);
    append_line(output, str);
}

/**
 * @brief Funkcia na genrovanie tela while konstrukcie
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
 * @param counter Pocitadlo konstrukcii
*/
void gen_while_body(string_t *output, int counter){
    append_line(output, "POPS GF@tmp_res\n"
                        "JUMPIFNEQ while_end");
    char str[16];
    sprintf(str, "%d", counter);
    append_line(output, str);
    append_line(output, " GF@tmp_res bool@true\n");
}

/**
 * @brief Funckia na generovanie konca while konstrukcie
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
 * @param counter Pocitadlo konstrukcii
*/
void gen_while_end(string_t *output, int counter){
    nestLevel--;
    if (nestLevel == 0){
        dstringFree(localVariables);
    }
    append_line(output, "LABEL while_end");
    char str[16];
    sprintf(str, "%d\n", counter);
    append_line(output, str);
    append_line(output, "POPFRAME\n");
}

// builtin functions

/**
 * @brief Funkcia na citanie stringu zo stdin
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
*/
void gen_read_str(string_t *output){
    append_line(output, "# func readString\n"
                "LABEL readString\n"
                "CREATEFRAME\n"
                "PUSHFRAME\n"
                "DEFVAR LF@str\n"
                "READ LF@str string\n"
                "PUSHS LF@str\n"
                "POPFRAME\n"
                "RETURN\n");
}

/**
 * @brief Funckia na citanie integer-u zo stdin
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
*/
void gen_read_int(string_t *output){
    append_line(output, "# func readINT\n"
                "LABEL readInt\n"
                "CREATEFRAME\n"
                "PUSHFRAME\n"
                "DEFVAR LF@inte\n"
                "READ LF@inte int\n"
                "PUSHS LF@inte\n"
                "POPFRAME\n"
                "RETURN\n");
}

/**
 * @brief Funkcia na citanie double-u zo stdin
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
*/
void gen_read_doub(string_t *output){
    append_line(output, "# func readFloat\n"
                "LABEL readDouble\n"
                "CREATEFRAME\n"
                "PUSHFRAME\n"
                "DEFVAR LF@doub\n"
                "READ LF@doub float\n"
                "PUSHS LF@doub\n"
                "POPFRAME\n"
                "RETURN\n");
}

/**
 * @brief Funkcia na vytlacenie argumentu/ov na stdout
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
*/
void gen_write(string_t *output){
    append_line(output, "# func write\n"
                        "LABEL write\n"
                        "CREATEFRAME\n"
                        "PUSHFRAME\n"
                        "DEFVAR LF@str\n"
                        "POPS LF@str\n"
                        "WRITE LF@str\n"
                        "POPFRAME\n"
                        "RETURN\n");
}

/**
 * @brief Funkcia na transformaciu int na double
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
*/
void gen_int2double(string_t *output){
    append_line(output, "# func Int2Double\n"
                        "LABEL Int2Double\n"
                        "INT2FLOATS\n"
                        "RETURN\n");
}

/**
 * @brief Funckia na trnasformaciu double na int
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
*/
void gen_double2int(string_t *output){
    append_line(output, "# func Double2Int\n"
                        "LABEL Double2Int\n"
                        "FLOAT2INTS\n"
                        "RETURN\n");
}

/**
 * @brief Funkcia na ziskanie dlzky string-u
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
*/
void gen_length(string_t *output){
    append_line(output, "# func length\n"
                        "LABEL length\n"
                        "CREATEFRAME\n"
                        "PUSHFRAME\n"
                        "DEFVAR LF@str\n"
                        "DEFVAR LF@len\n"
                        "POPS LF@str\n"
                        "STRLEN LF@len LF@str\n"
                        "PUSHS LF@len\n"
                        "POPFRAME\n"
                        "RETURN\n");
}

/**
 * @brief Funkcia na vytvorenie substringu
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
*/
void gen_substring(string_t *output){
    append_line(output, "# func substring\n"
                        "LABEL substring\n"
                        "CREATEFRAME\n"
                        "PUSHFRAME\n"
                        "DEFVAR LF@i\n"
                        "DEFVAR LF@j\n"
                        "DEFVAR LF@str\n"
                        "DEFVAR LF@len\n"
                        "DEFVAR LF@res\n"
                        "DEFVAR LF@substr\n"
                        "DEFVAR LF@tmp\n"
                        "POPS LF@j\n"
                        "POPS LF@i\n"
                        "POPS LF@str\n"
                        "STRLEN LF@len LF@str\n"
                        // i < 0
                        "LT LF@res LF@i int@0\n"
                        "JUMPIFEQ substring_err LF@res bool@true\n"
                        // j < 0
                        "LT LF@res LF@j int@0\n"
                        "JUMPIFEQ substring_err LF@res bool@true\n"
                        // i > j
                        "GT LF@res LF@i LF@j\n"
                        "JUMPIFEQ substring_err LF@res bool@true\n"
                        // i >= len
                        "GT LF@res LF@i LF@len\n"
                        "JUMPIFEQ substring_err LF@res bool@true\n"
                        "JUMPIFEQ substring_err LF@i LF@len\n"
                        // j > len
                        "GT LF@res LF@j LF@len\n"
                        "JUMPIFEQ substring_err LF@res bool@true\n"
                        "MOVE LF@substr string@nil\n"
                        //cycle
                        "LABEL substring_cycle\n"
                        "SUB LF@res LF@j LF@i\n"
                        "JUMPIFEQ substring_cycle_end LF@tmp int@1\n"
                        "GETCHAR LF@tmp LF@str LF@i\n"
                        "CONCAT LF@substr LF@substr LF@tmp\n"
                        "ADD LF@i LF@i int@1\n"
                        "JUMP substring_cycle\n"
                        "LABEL substring_cycle_end\n"
                        "PUSHS LF@substr\n"
                        "JUMP substring_end\n"
                        "LABEL substring_err\n"
                        "PUSHS nil@nil\n"
                        "LABEL substring_end\n"
                        "POPFRAME\n"
                        "RETURN\n");
}

/**
 * @brief Funkcia na ziskanie ordinalnej hodnoty prveho znaku v stringu
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
*/
void gen_ord(string_t *output){
    append_line(output, "# func ord\n"
                        "LABEL ord\n"
                        "CREATEFRAME\n"
                        "PUSHFRAME\n"
                        "DEFVAR LF@str\n"
                        "DEFVAR LF@ordin\n"
                        "POPS LF@str\n"
                        "STRI2INT LF@ordin LF@str int@0\n"
                        "PUSHS LF@ordin\n"
                        "POPFRAME\n"
                        "RETURN\n");
}

/**
 * @brief Funkcia na ziskanie prveho znaku v stringu
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
*/
void gen_chr(string_t *output){
    append_line(output, "# func chr\n"
                        "LABEL chr\n"
                        "CREATEFRAME\n"
                        "PUSHFRAME\n"
                        "DEFVAR LF@ordin\n"
                        "DEFVAR LF@ch\n"
                        "POPS LF@ordin\n"
                        "INT2CHAR LF@ch LF@ordin\n"
                        "PUSHS LF@ch\n"
                        "POPFRAME\n"
                        "RETURN\n");
}

// pomocne funkcie

/**
 * @brief Pomocna funkcia na zistenie rovnosti
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
*/
void gen_eq(string_t *output){
    append_line(output, "# function equal\n"
                        "LABEL equals\n"
                        "CREATEFRAME\n"
                        "PUSHFRAME\n"
                        "DEFVAR LF@op0\n"
                        "DEFVAR LF@op1\n"
                        "POPS LF@op1\n"
                        "POPS LF@op0\n"
                        "JUMPIFEQ eq_true LF@op0 LF@op1\n"
                        "PUSHS bool@false\n"
                        "JUMP eq_end\n"
                        "LABEL eq_true\n"
                        "PUSHS bool@true\n"
                        "LABEL eq_end\n"
                        "POPFRAME\n"
                        "RETURN\n");
}

/**
 * @brief Pomocna funkcia na zistenie nerovnosti
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
*/
void gen_neq(string_t *output){
    append_line(output, "# function not equal\n"
                        "LABEL nequals\n"
                        "CREATEFRAME\n"
                        "PUSHFRAME\n"
                        "DEFVAR LF@op0\n"
                        "DEFVAR LF@op1\n"
                        "POPS LF@op1\n"
                        "POPS LF@op0\n"
                        "JUMPIFEQ neq_true LF@op0 LF@op1\n"
                        "PUSHS bool@true\n"
                        "JUMP neq_end\n"
                        "LABEL neq_true\n"
                        "PUSHS bool@false\n"
                        "LABEL neq_end\n"
                        "POPFRAME\n"
                        "RETURN\n");
}

/**
 * @brief Pomocna funkcia na zistenie, ci je lavy operand mensi ako pravy
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
*/
void gen_lesser(string_t *output){
    append_line(output, "# function lesser\n"
                        "LABEL lesser\n"
                        "CREATEFRAME\n"
                        "PUSHFRAME\n"
                        "DEFVAR LF@op0\n"
                        "DEFVAR LF@op1\n"
                        "DEFVAR LF@res\n"
                        "POPS LF@op1\n"
                        "POPS LF@op0\n"
                        "LT LF@res LF@op0 LF@op1\n"
                        "PUSHS LF@res\n"
                        "POPFRAME\n"
                        "RETURN\n");
}

/**
 * @brief Poomocna funkcia na zistenie, ci je lavy operand mensi alebo rovny ako pravy
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
*/
void gen_lesser_or_eq(string_t *output){
    append_line(output, "# function lesser or equal\n"
                        "LABEL lesser_or_equal\n"
                        "CREATEFRAME\n"
                        "PUSHFRAME\n"
                        "DEFVAR LF@op0\n"
                        "DEFVAR LF@op1\n"
                        "DEFVAR LF@res0\n"
                        "DEFVAR LF@res1\n"
                        "POPS LF@op1\n"
                        "POPS LF@op0\n"
                        "LT LF@res0 LF@op0 LF@op1\n"
                        "EQ LF@res1 LF@op0 LF@op1\n"
                        "PUSHS LF@res0\n"
                        "PUSHS LF@res1\n"
                        "ORS\n"
                        "POPFRAME\n"
                        "RETURN\n");
}

/**
 * @brief Pomocna funkcia na zistenie, ci je lavy operand vacsi ako pravy
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
*/
void gen_greater(string_t *output){
    append_line(output, "# function greater\n"
                        "LABEL greater\n"
                        "CREATEFRAME\n"
                        "PUSHFRAME\n"
                        "DEFVAR LF@op0\n"
                        "DEFVAR LF@op1\n"
                        "POPS LF@op1\n"
                        "POPS LF@op0\n"
                        "PUSHS LF@op1\n"
                        "PUSHS LF@op0\n"
                        "CALL lesser\n"
                        "POPFRAME\n"
                        "RETURN\n");
}

/**
 * @brief Pomocna funkcia na zistenie, ci je lavy operand vacsi alebo rovny ako pravy
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
*/
void gen_greater_or_eq(string_t *output){
    append_line(output, "# function greater or equal\n"
                        "LABEL greater_or_equal\n"
                        "CREATEFRAME\n"
                        "PUSHFRAME\n"
                        "DEFVAR LF@op0\n"
                        "DEFVAR LF@op1\n"
                        "POPS LF@op1\n"
                        "POPS LF@op0\n"
                        "PUSHS LF@op1\n"
                        "PUSHS LF@op0\n"
                        "CALL lesser_or_equal\n"
                        "POPFRAME\n"
                        "RETURN\n");
}

/**
 * @brief Pomocna funkcia na konkatenaciu
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
*/
void gen_concat(string_t *output){
    append_line(output, "# concatenation\n"
                        "LABEL concat\n"
                        "CREATEFRAME\n"
                        "PUSHFRAME\n"
                        "DEFVAR LF@op0\n"
                        "DEFVAR LF@op1\n"
                        "DEFVAR LF@res\n"
                        "POPS LF@op1\n"
                        "POPS LF@op0\n"
                        "CONCAT LF@res LF@op0 LF@op1\n"
                        "PUSHS LF@res\n"
                        "POPFRAME\n"
                        "RETURN\n");
}

/**
 * @brief Pomocna funkcia na odstranenie nil
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
*/
void gen_questionm(string_t *output){
    append_line(output, "# function remove nil\n"
                        "LABEL remove_nil\n"
                        "CREATEFRAME\n"
                        "PUSHFRAME\n"
                        "DEFVAR LF@op0\n"
                        "DEFVAR LF@op1\n"
                        "DEFVAR LF@type0\n"
                        "DEFVAR LF@type1\n"
                        "POPS LF@op1\n"
                        "POPS LF@op0\n"
                        "JUMPIFEQ first_nil LF@op0 nil@nil\n"
                        "PUSHS LF@op0\n"
                        "POPFRAME\n"
                        "RETURN\n"
                        "LABEL first_nil\n"
                        "TYPE LF@type0 LF@op0\n"
                        "TYPE LF@type1 LF@op1\n"
                        "JUMPIFEQ nil_same LF@type0 LF@type1\n"
                        "JUMP error\n"
                        "LABEL nil_same\n"
                        "PUSHS LF@op1\n"
                        "POPFRAME\n"
                        "RETURN\n");
}

/**
 * @brief Pomocna funkcia na pripravu hodnot pre dalsie spracovanie
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
*/
void prepare_values(string_t *output){
    append_line(output, "# prepare values\n"
                        "LABEL prepare\n"
                        "CREATEFRAME\n"
                        "PUSHFRAME\n"
                        "DEFVAR LF@op0\n"
                        "DEFVAR LF@op1\n"
                        "POPS LF@op0\n"
                        "POPS LF@op1\n"
                        "POPFRAME\n"
                        "RETURN\n");
}

void gen_true(string_t *output){
    append_line(output, "PUSHS bool@true\n");
}

void gen_false(string_t *output){
    append_line(output, "PUSHS bool@false\n");
}