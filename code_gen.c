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
extern int global_counter;
extern int local_counter;
extern Stack counter_stack;

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

void push_counter(int counter){
    token_t newToken;
    initToken(&newToken);
    newToken.attribute.intValue = counter;
    Stack_Push(&counter_stack, &newToken);
}

int top_counter(){
    int top = counter_stack.elements[counter_stack.topIndex].attribute.intValue;
    return top;
}

void pop_counter(){
    token_t deleteToken;
    Stack_Top(&counter_stack, &deleteToken);
    Stack_Pop(&counter_stack);
    freeToken(&deleteToken);
}

/**
 * @brief Funkcia na vytvorenie zaciatku generovaneho outputu
 * 
 * @return Ukazatel na dynamicky string, v ktorom je output
*/
string_t *gen_start(){
    string_t *output = new_line(".IFJcode23\n");
    append_line(output, "DEFVAR GF@tmp_res\n"
                        "DEFVAR GF@type\n"
                        "DEFVAR GF@first_def\n"
                        "MOVE GF@first_def bool@true\n"
                        "CREATEFRAME\n");
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
    prepare_values(output);
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
    fprintf(stdout, "%s", output->data);
    dstringFree(output);
    free(output);
}

/**
 * @brief Funkcia na nacitanie hodnoty
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
 * @param token Token, ktoreho hodnota sa ma nacitat
*/
void gen_value(string_t *output, token_t *token, bool isVariable, char* name){
    if (isVariable){
        char *isDefined = NULL;
        if (localVariables != NULL){
            char varname[16];
            sprintf(varname, "TF@%s\n", name);
            isDefined = strstr(localVariables->data, varname);
        }
        if (nestLevel == 0 || isDefined == NULL){
            append_line(output, "PUSHS GF@");
        }
        else{
            append_line(output, "PUSHS TF@");
        }
        append_line(output, name);
        append_line(output, "\n");
    }
    else{
        switch (token->type) {
            case TOK_KW_NIL: {
                append_line(output, "PUSHS nil@nil\n");
                break;
            }
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
                        append_line(output, "\\");
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
void gen_var(string_t *output, char *name, bool includesNil, token_type_t type){
    global_counter++;
    local_counter = global_counter;
    char str[16];
    sprintf(str, "%d", local_counter);
    append_line(output, "JUMPIFNEQ var_end");
    append_line(output, str);
    append_line(output, " GF@first_def bool@true\n");
    if (nestLevel == 0){
        append_line(output, "DEFVAR GF@");
        append_line(output, name);
        append_line(output, "\n");
        if (includesNil){
            append_line(output, "PUSHS nil@nil\n");
            gen_assign(output, name, type);
        }
    }
    else{
        char *isDefined = NULL;
        if (localVariables != NULL){
            char varname[16];
            sprintf(varname, "TF@%s\n", name);
            isDefined = strstr(localVariables->data, varname);
            append_line(localVariables, "DEFVAR TF@");
            append_line(localVariables, name);
            append_line(localVariables, "\n");
            append_line(localVariables, "MOVE TF@");
            append_line(localVariables, name);
            append_line(localVariables, " LF@");
            append_line(localVariables, name);
            append_line(localVariables, "\n");
        }
        if (isDefined == NULL){
            append_line(output, "DEFVAR TF@");
            append_line(output, name);
            append_line(output, "\n");
            if (includesNil){
                append_line(output, "PUSHS nil@nil\n");
                gen_assign(output, name, type);
            }
        }
    }
    append_line(output, "LABEL var_end");
    append_line(output, str);
    append_line(output, "\n");
}

/**
 * @brief Funckia na priradenie hodnoty premennnej
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
 * @param token Token, ktoreho meno udava nazov premennej
 * @param function Bool, ktory udava, ci premmenna je vo funkcii (true - vo funkcii; false - v maine)
*/
void gen_assign(string_t *output, char *name, token_type_t type){
    global_counter++;
    local_counter = global_counter;
    char str[16];
    sprintf(str, "%d", local_counter);
    char *isDefined = NULL;
    if (localVariables != NULL){
        char varname[16];
        sprintf(varname, "TF@%s\n", name);
        isDefined = strstr(localVariables->data, varname);
    }
    if (nestLevel == 0 || isDefined == NULL){
        append_line(output, "POPS GF@");
        append_line(output, name);
        append_line(output, "\n");
        append_line(output, "TYPE GF@type GF@");
        append_line(output, name);
        append_line(output, "\n"
                            "JUMPIFEQ trans_not");
        append_line(output, str);
        append_line(output, " GF@type ");
        if (type == TOK_INT){
            append_line(output, "string@int\n"
                                "JUMPIFNEQ trans_not");
            append_line(output, str);
            append_line(output, " GF@type string@float\n"
                                "FLOAT2INT GF@");
            append_line(output, name);
            append_line(output, " GF@");
            append_line(output, name);
            append_line(output, "\n");
        }
        else if (type == TOK_DOUBLE){
            append_line(output, "string@float\n"
                                "JUMPIFNEQ trans_not");
            append_line(output, str);
            append_line(output, " GF@type string@int\n"
                                "INT2FLOAT GF@");
            append_line(output, name);
            append_line(output, " GF@");
            append_line(output, name);
            append_line(output, "\n");
        }
        else{
            append_line(output, "GF@type\n");
        }
        append_line(output, "LABEL trans_not");
        append_line(output, str);
        append_line(output, "\n");
    }
    else{
        append_line(output, "POPS TF@");
        append_line(output, name);
        append_line(output, "\n");
        append_line(output, "TYPE GF@type TF@");
        append_line(output, name);
        append_line(output, "\n"
                            "JUMPIFEQ trans_not");
        append_line(output, str);
        append_line(output, " GF@type ");
        if (type == TOK_INT){
            append_line(output, "string@int\n"
                                "JUMPIFNEQ trans_not");
            append_line(output, str);
            append_line(output, " GF@type string@float\n"
                                "FLOAT2INT TF@");
            append_line(output, name);
            append_line(output, " TF@");
            append_line(output, name);
            append_line(output, "\n");
        }
        else if (type == TOK_DOUBLE){
            append_line(output, "string@float\n"
                                "JUMPIFNEQ trans_not");
            append_line(output, str);
            append_line(output, " GF@type string@int\n"
                                "INT2FLOAT TF@");
            append_line(output, name);
            append_line(output, " TF@");
            append_line(output, name);
            append_line(output, "\n");
        }
        else{
            append_line(output, "GF@type\n");
        }
        append_line(output, "LABEL trans_not");
        append_line(output, str);
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
    append_line(output, "\nPUSHFRAME\n"
                        "CREATEFRAME\n");
    if (nestLevel == 1){
        localVariables = new_line("");
    }
    else{
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
        free(localVariables);
        localVariables = NULL;
    }
    append_line(output, "POPFRAME\n"
                        "RETURN\n"
                        "LABEL ");
    append_line(output, token->attribute.str.data);
    append_line(output, "_end\n");
}

void gen_func_return(string_t *output, token_t *token){
    if (token->type == TOK_EOL){
        append_line(output, "POPFRAME\n"
                            "RETURN\n");
    }
    else{
        if (token->type == TOK_IDENTIFIER){
            gen_value(output, NULL, true, token->attribute.str.data);
        }
        else{
            gen_value(output, token, false, NULL);
        }
    }
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
            case TOK_KW_NIL: {
                gen_value(output, &(items->nodes[index]->token), false, NULL);
                break;
            }
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
                append_line(output, "CALL prepare\n"
                                    "MULS\n");
                break;
            }
            case TOK_DIV:{
                if (items->nodes[index]->left->type == TOK_INT){
                    append_line(output, "IDIVS\n");
                }
                else{
                    append_line(output, "DIVS\n");
                }
                break;
            }
            case TOK_PLUS:{
                if (isString){
                    append_line(output, "CALL _concat\n");
                }
                else{
                    append_line(output, "CALL prepare\n"
                                        "ADDS\n");
                }
                break;
            }
            case TOK_MINUS:{
                append_line(output, "CALL prepare\n"
                                    "SUBS\n");
                break;
            }
            case TOK_ASSIGN:{
                append_line(output, "CALL equals\n");
                break;
            }
            case TOK_EQUAL:{
                append_line(output, "CALL prepare\n"
                                    "CALL equals\n");
                break;
            }
            case TOK_NOT_EQUAL:{
                append_line(output, "CALL prepare\n"
                                    "CALL nequals\n");
                break;
            }
            case TOK_LESSER:{
                append_line(output, "CALL prepare\n"
                                    "CALL lesser\n");
                break;
            }
            case TOK_LESSER_OR_EQUAL:{
                append_line(output, "CALL prepare\n"
                                    "CALL lesser_or_equal\n");
                break;
            }
            case TOK_GREATER:{
                append_line(output, "CALL prepare\n"
                                    "CALL greater\n");
                break;
            }
            case TOK_GREATER_OR_EQUAL:{
                append_line(output, "CALL prepare\n"
                                    "CALL greater_or_equal\n");
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
 * @brief Funkcia na generovanie zaciatku if konstrukcie
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
*/
void gen_if(string_t *output){
    global_counter++;
    local_counter = global_counter;
    push_counter(local_counter);
    nestLevel++;
    append_line(output, "# if condition\n"
                        "POPS GF@tmp_res\n"
                        "JUMPIFNEQ else");
    char str[16];
    sprintf(str, "%d", local_counter);
    append_line(output, str);
    append_line(output, " GF@tmp_res bool@true\n");
    append_line(output, "# body of if\n");
    append_line(output, "PUSHFRAME\n"
                        "CREATEFRAME\n");
    if (nestLevel == 1){
        localVariables = new_line("");
    }
    else{
        append_line(output, localVariables->data);
    }
}

void gen_if_let(string_t *output, char *name){
    global_counter++;
    local_counter = global_counter;
    append_line(output, "PUSHFRAME\n"
                        "CREATEFRAME\n"
                        "DEFVAR TF@type\n"
                        "TYPE TF@type ");
    char *isDefined = NULL;
    if (localVariables != NULL){
        char varname[16];
        sprintf(varname, "TF@%s\n", name);
        isDefined = strstr(localVariables->data, varname);
    }

    if (isDefined == NULL){
        append_line(output, "GF@");
    }
    else{
        append_line(output, "TF@");
    }
    append_line(output, name);
    char str[16];
    sprintf(str, "%d", local_counter);
    append_line(output, "\nJUMPIFEQ let_nil");
    append_line(output, str);
    append_line(output, " TF@type string@nil\n"
                        "PUSHS bool@true\n"
                        "JUMP let_nil_end");
    append_line(output, str);
    append_line(output, "\nLABEL let_nil");
    append_line(output, str);
    append_line(output, "\nPUSHS bool@false\n"
                        "LABEL let_nil_end");
    append_line(output, str);
    append_line(output, "\nPOPFRAME\n");
}

/**
 * @brief Funkcia na vygenerovanie else
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
*/
void gen_else(string_t *output){
    local_counter = top_counter();
    append_line(output, "JUMP if_end");
    char str[16];
    sprintf(str, "%d\n", local_counter);
    append_line(output, str);
    append_line(output, "# body of else\n"
                        "LABEL else");
    append_line(output, str);
    append_line(output, "PUSHFRAME\n"
                        "CREATEFRAME\n");
}

/**
 * @brief Funkcia na generovania konca if-else konstrukcie
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
*/
void gen_if_end(string_t *output){
    local_counter = top_counter();
    nestLevel--;
    if (nestLevel == 0){
        dstringFree(localVariables);
        free(localVariables);
        localVariables = NULL;
    }
    append_line(output, "LABEL if_end");
    char str[16];
    sprintf(str, "%d\n", local_counter);
    append_line(output, str);
    append_line(output, "POPFRAME\n");
    pop_counter();
}

/**
 * @brief Funckia na genrovanie zaciatku while konstrukcie
 * @param output Ukazatel na dynamicky string, v ktorom je output
*/
void gen_while(string_t *output){
    global_counter++;
    local_counter = global_counter;
    push_counter(local_counter);
    nestLevel++;
    append_line(output, "PUSHFRAME\n"
                        "CREATEFRAME\n"
                        "MOVE GF@first_def bool@true\n");
    if (nestLevel == 1){
        localVariables = new_line("");
    }
    else{
        append_line(output, localVariables->data);
    }
    append_line(output, "LABEL while");
    char str[16];
    sprintf(str, "%d\n", local_counter);
    append_line(output, str);
}

/**
 * @brief Funkcia na genrovanie tela while konstrukcie
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
*/
void gen_while_body(string_t *output){
    local_counter = top_counter();
    append_line(output, "POPS GF@tmp_res\n"
                        "JUMPIFNEQ while_end");
    char str[16];
    sprintf(str, "%d", local_counter);
    append_line(output, str);
    append_line(output, " GF@tmp_res bool@true\n");
}

/**
 * @brief Funckia na generovanie konca while konstrukcie
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
*/
void gen_while_end(string_t *output){
    local_counter = top_counter();
    nestLevel--;
    if (nestLevel == 0){
        dstringFree(localVariables);
        free(localVariables);
        localVariables = NULL;
    }

    char str[16];
    sprintf(str, "%d\n", local_counter);
    append_line(output, "MOVE GF@first_def bool@false\n"
                        "JUMP while");
    append_line(output, str);
    append_line(output, "LABEL while_end");
    append_line(output, str);
    append_line(output, "POPFRAME\n"
                        "MOVE GF@first_def bool@true\n");
    pop_counter();
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
                "PUSHFRAME\n"
                "CREATEFRAME\n"
                "DEFVAR TF@str\n"
                "READ TF@str string\n"
                "PUSHS TF@str\n"
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
                "PUSHFRAME\n"
                "CREATEFRAME\n"
                "DEFVAR TF@inte\n"
                "READ TF@inte int\n"
                "PUSHS TF@inte\n"
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
                "PUSHFRAME\n"
                "CREATEFRAME\n"
                "DEFVAR TF@doub\n"
                "READ TF@doub float\n"
                "PUSHS TF@doub\n"
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
                        "PUSHFRAME\n"
                        "CREATEFRAME\n"
                        "DEFVAR TF@str\n"
                        "POPS TF@str\n"
                        "WRITE TF@str\n"
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
                        "PUSHFRAME\n"
                        "CREATEFRAME\n"
                        "DEFVAR TF@str\n"
                        "DEFVAR TF@len\n"
                        "POPS TF@str\n"
                        "STRLEN TF@len TF@str\n"
                        "PUSHS TF@len\n"
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
                        "PUSHFRAME\n"
                        "CREATEFRAME\n"
                        "DEFVAR TF@i\n"
                        "DEFVAR TF@j\n"
                        "DEFVAR TF@str\n"
                        "DEFVAR TF@len\n"
                        "DEFVAR TF@res\n"
                        "DEFVAR TF@substr\n"
                        "DEFVAR TF@tmp\n"
                        "POPS TF@j\n"
                        "POPS TF@i\n"
                        "POPS TF@str\n"
                        "STRLEN TF@len TF@str\n"
                        // i < 0
                        "LT TF@res TF@i int@0\n"
                        "JUMPIFEQ substring_err TF@res bool@true\n"
                        // j < 0
                        "LT TF@res TF@j int@0\n"
                        "JUMPIFEQ substring_err TF@res bool@true\n"
                        // i > j
                        "GT TF@res TF@i TF@j\n"
                        "JUMPIFEQ substring_err TF@res bool@true\n"
                        // i >= len
                        "GT TF@res TF@i TF@len\n"
                        "JUMPIFEQ substring_err TF@res bool@true\n"
                        "JUMPIFEQ substring_err TF@i TF@len\n"
                        // j > len
                        "GT TF@res TF@j TF@len\n"
                        "JUMPIFEQ substring_err TF@res bool@true\n"
                        "MOVE TF@substr string@\n"
                        //cycle
                        "LABEL substring_cycle\n"
                        "SUB TF@res TF@j TF@i\n"
                        "JUMPIFEQ substring_cycle_end TF@res int@0\n"
                        "GETCHAR TF@tmp TF@str TF@i\n"
                        "CONCAT TF@substr TF@substr TF@tmp\n"
                        "ADD TF@i TF@i int@1\n"
                        "JUMP substring_cycle\n"
                        "LABEL substring_cycle_end\n"
                        "PUSHS TF@substr\n"
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
                        "PUSHFRAME\n"
                        "CREATEFRAME\n"
                        "DEFVAR TF@str\n"
                        "DEFVAR TF@ordin\n"
                        "POPS TF@str\n"
                        "STRI2INT TF@ordin TF@str int@0\n"
                        "PUSHS TF@ordin\n"
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
                        "PUSHFRAME\n"
                        "CREATEFRAME\n"
                        "DEFVAR TF@ordin\n"
                        "DEFVAR TF@ch\n"
                        "POPS TF@ordin\n"
                        "INT2CHAR TF@ch TF@ordin\n"
                        "PUSHS TF@ch\n"
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
                        "PUSHFRAME\n"
                        "CREATEFRAME\n"
                        "DEFVAR TF@op0\n"
                        "DEFVAR TF@op1\n"
                        "POPS TF@op1\n"
                        "POPS TF@op0\n"
                        "JUMPIFEQ eq_true TF@op0 TF@op1\n"
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
                        "PUSHFRAME\n"
                        "CREATEFRAME\n"
                        "DEFVAR TF@op0\n"
                        "DEFVAR TF@op1\n"
                        "POPS TF@op1\n"
                        "POPS TF@op0\n"
                        "JUMPIFEQ neq_true TF@op0 TF@op1\n"
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
                        "PUSHFRAME\n"
                        "CREATEFRAME\n"
                        "DEFVAR TF@op0\n"
                        "DEFVAR TF@op1\n"
                        "DEFVAR TF@res\n"
                        "POPS TF@op1\n"
                        "POPS TF@op0\n"
                        "LT TF@res TF@op0 TF@op1\n"
                        "PUSHS TF@res\n"
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
                        "PUSHFRAME\n"
                        "CREATEFRAME\n"
                        "DEFVAR TF@op0\n"
                        "DEFVAR TF@op1\n"
                        "DEFVAR TF@res0\n"
                        "DEFVAR TF@res1\n"
                        "POPS TF@op1\n"
                        "POPS TF@op0\n"
                        "LT TF@res0 TF@op0 TF@op1\n"
                        "EQ TF@res1 TF@op0 TF@op1\n"
                        "PUSHS TF@res0\n"
                        "PUSHS TF@res1\n"
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
                        "PUSHFRAME\n"
                        "CREATEFRAME\n"
                        "DEFVAR TF@op0\n"
                        "DEFVAR TF@op1\n"
                        "POPS TF@op1\n"
                        "POPS TF@op0\n"
                        "PUSHS TF@op1\n"
                        "PUSHS TF@op0\n"
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
                        "PUSHFRAME\n"
                        "CREATEFRAME\n"
                        "DEFVAR TF@op0\n"
                        "DEFVAR TF@op1\n"
                        "POPS TF@op1\n"
                        "POPS TF@op0\n"
                        "PUSHS TF@op1\n"
                        "PUSHS TF@op0\n"
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
                        "LABEL _concat\n"
                        "PUSHFRAME\n"
                        "CREATEFRAME\n"
                        "DEFVAR TF@op0\n"
                        "DEFVAR TF@op1\n"
                        "DEFVAR TF@res\n"
                        "POPS TF@op1\n"
                        "POPS TF@op0\n"
                        "CONCAT TF@res TF@op0 TF@op1\n"
                        "PUSHS TF@res\n"
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
                        "PUSHFRAME\n"
                        "CREATEFRAME\n"
                        "DEFVAR TF@op0\n"
                        "DEFVAR TF@op1\n"
                        "POPS TF@op1\n"
                        "POPS TF@op0\n"
                        "JUMPIFEQ first_nil TF@op0 nil@nil\n"
                        "PUSHS TF@op0\n"
                        "POPFRAME\n"
                        "RETURN\n"
                        "LABEL first_nil\n"
                        "PUSHS TF@op1\n"
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
                        "PUSHFRAME\n"
                        "CREATEFRAME\n"
                        "DEFVAR TF@op0\n"
                        "DEFVAR TF@op1\n"
                        "DEFVAR TF@type0\n"
                        "DEFVAR TF@type1\n"
                        "POPS TF@op1\n"
                        "POPS TF@op0\n"
                        "TYPE TF@type0 TF@op0\n"
                        "TYPE TF@type1 TF@op1\n"
                        "JUMPIFEQ prepare_end TF@type0 string@string\n"
                        "JUMPIFEQ prepare_end TF@type0 TF@type1\n"
                        "JUMPIFEQ prepare0 TF@type0 string@int\n"
                        "INT2FLOAT TF@op1 TF@op1\n"
                        "JUMP prepare_end\n"
                        "LABEL prepare0\n"
                        "INT2FLOAT TF@op0 TF@op0\n"
                        "LABEL prepare_end\n"
                        "PUSHS TF@op0\n"
                        "PUSHS TF@op1\n"
                        "POPFRAME\n"
                        "RETURN\n");
}