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

void newTF(string_t *output){
    char *token;
    char *str = malloc(sizeof(char) * localVariables->length);
    sprintf(str, "%s", localVariables->data);
    token = strtok(str, "\n");

    while (token != NULL){
        append_line(output, "DEFVAR TF@");
        append_line(output, token);
        append_line(output, "\nMOVE TF@");
        append_line(output, token);
        append_line(output, " LF@");
        append_line(output, token);
        append_line(output, "\n");

        token = strtok(NULL, "\n");
    }
    free(str);
}

void updateValues(string_t *output){
    char *token;
    char *str = malloc(sizeof(char) * localVariables->length);
    sprintf(str, "%s", localVariables->data);
    char var[16];
    sprintf(var, "$%d", nestLevel);
    char *isDefined;
    token = strtok(str, "\n");

    while (token != NULL){
        isDefined = NULL;
        isDefined = strstr(token, var);
        if (isDefined != NULL){
            append_line(output, "\nMOVE LF@");
            append_line(output, token);
            append_line(output, " TF@");
            append_line(output, token);
            append_line(output, "\n");
        }

        token = strtok(NULL, "\n");
    }
    free(str);
}

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

void gen_end(string_t *output){
    append_line(output, "EXIT int@0\n");
    fprintf(stdout, "%s", output->data);
    dstringFree(output);
    free(output);
}

void gen_value(string_t *output, token_t *token, bool isVariable, char* name){
    if (isVariable){
        char *newName = NULL;
        char varname[16];
        char searchName[16];
        if (localVariables != NULL){
            char *tmp = NULL;
            for (int i = 0; i <= nestLevel; i++){
                sprintf(varname, "%s$%d", name, i);
                sprintf(searchName, "\n%s$%d\n", name, i);
                tmp = strstr(localVariables->data, searchName);
                if (tmp != NULL){
                    char varname2[16];
                    sprintf(varname2, "%s", varname);
                    newName = varname2;
                }
            }
        }
        if (nestLevel == 0 || newName == NULL){
            append_line(output, "PUSHS GF@");
            append_line(output, name);
            append_line(output, "\n");
        }
        else{
            append_line(output, "PUSHS TF@");
            append_line(output, newName);
            append_line(output, "\n");
        }
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

void gen_var(string_t *output, char *name, bool includesNil, token_type_t type){
    bool isDefined = false;
    char varname[16];
    char searchName[16];
    if (localVariables != NULL){
        char *tmp = NULL;
        for (int i = 0; i <= nestLevel; i++){
            sprintf(varname, "%s$%d", name, i);
            sprintf(searchName, "\n%s$%d\n", name, i);
            tmp = strstr(localVariables->data, searchName);
            if (tmp != NULL){
                isDefined = true;
            }
        }
    }
    if (isDefined){
        return;
    }
    global_counter++;
    local_counter = global_counter;
    char str[16];
    sprintf(str, "%d", local_counter);
    char newName[16];
    sprintf(newName, "%s$%d", name, nestLevel);
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
        append_line(output, "DEFVAR TF@");
        append_line(output, newName);
        append_line(output, "\n");
        append_line(localVariables, newName);
        append_line(localVariables, "\n");
        if (includesNil){
            append_line(output, "PUSHS nil@nil\n");
            gen_assign(output, name, type);
        }
    }
    append_line(output, "LABEL var_end");
    append_line(output, str);
    append_line(output, "\n");
}

void gen_assign(string_t *output, char *name, token_type_t type){
    global_counter++;
    local_counter = global_counter;
    char str[16];
    sprintf(str, "%d", local_counter);
    char *newName = NULL;
    char varname[16];
    char searchName[16];
    if (localVariables != NULL){
        char *tmp = NULL;
        for (int i = 0; i <= nestLevel; i++){
            sprintf(varname, "%s$%d", name, i);
            sprintf(searchName, "\n%s$%d\n", name, i);
            tmp = strstr(localVariables->data, searchName);
            if (tmp != NULL){
                char varname2[16];
                sprintf(varname2, "%s", varname);
                newName = varname2;
            }
        }
    }
    if (nestLevel == 0 || newName == NULL){
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
        append_line(output, newName);
        append_line(output, "\n");
        append_line(output, "TYPE GF@type TF@");
        append_line(output, newName);
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
            append_line(output, newName);
            append_line(output, " TF@");
            append_line(output, newName);
            append_line(output, "\n");
        }
        else if (type == TOK_DOUBLE){
            append_line(output, "string@float\n"
                                "JUMPIFNEQ trans_not");
            append_line(output, str);
            append_line(output, " GF@type string@int\n"
                                "INT2FLOAT TF@");
            append_line(output, newName);
            append_line(output, " TF@");
            append_line(output, newName);
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

void gen_func(string_t *output, token_t *token){
    nestLevel++;
    append_line(output, "JUMP ");
    append_line(output, token->attribute.str.data);
    append_line(output, "_end\n");
    append_line(output, "LABEL ");
    append_line(output, token->attribute.str.data);
    append_line(output, "\nPUSHFRAME\n"
                        "CREATEFRAME\n"
                        "MOVE GF@first_def bool@true\n");
    if (nestLevel == 1){
        localVariables = new_line("\n");
    }
    else{
        newTF(output);
    }
}

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

void gen_func_return(string_t *output){
    append_line(output, "POPFRAME\n"
                        "RETURN\n");
}

void gen_func_call(string_t *output, char *name){
    append_line(output, "CALL ");
    append_line(output, name);
    append_line(output, "\n");
}

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
                if (items->nodes[index]->type == TOK_STRING){
                    isString = true;
                }
                else{
                    isString = false;
                }
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
                append_line(output, "CALL prepare\n");
                if (items->nodes[index]->left->type == TOK_INT && items->nodes[index]->right->type == TOK_INT){
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
                        "CREATEFRAME\n"
                        "MOVE GF@first_def bool@true\n");
    if (nestLevel == 1){
        localVariables = new_line("\n");
    }
    else{
        newTF(output);
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
                        "CREATEFRAME\n"
                        "MOVE GF@first_def bool@true\n");
    if (nestLevel == 1){
        localVariables = new_line("\n");
    }
    else{
        newTF(output);
    }
}

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

void gen_while(string_t *output){
    global_counter++;
    local_counter = global_counter;
    push_counter(local_counter);
    nestLevel++;
    append_line(output, "PUSHFRAME\n"
                        "CREATEFRAME\n"
                        "MOVE GF@first_def bool@true\n");
    if (nestLevel == 1){
        localVariables = new_line("\n");
    }
    else{
        newTF(output);
    }
    append_line(output, "LABEL while");
    char str[16];
    sprintf(str, "%d\n", local_counter);
    append_line(output, str);
}

void gen_while_body(string_t *output){
    local_counter = top_counter();
    append_line(output, "POPS GF@tmp_res\n"
                        "JUMPIFNEQ while_end");
    char str[16];
    sprintf(str, "%d", local_counter);
    append_line(output, str);
    append_line(output, " GF@tmp_res bool@true\n");
}

void gen_while_end(string_t *output){
    local_counter = top_counter();
    nestLevel--;
    updateValues(output);
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

void gen_int2double(string_t *output){
    append_line(output, "# func Int2Double\n"
                        "LABEL Int2Double\n"
                        "INT2FLOATS\n"
                        "RETURN\n");
}

void gen_double2int(string_t *output){
    append_line(output, "# func Double2Int\n"
                        "LABEL Double2Int\n"
                        "FLOAT2INTS\n"
                        "RETURN\n");
}

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