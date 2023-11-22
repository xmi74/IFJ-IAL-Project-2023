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
    gen_write(output);
    gen_int2double(output);
    gen_double2int(output);
    gen_length(output);
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

/**
 * @brief Funkcia na vytvorenie premennej
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
 * @param token Token, ktoreho meno udava nazov premennej
 * @param function Bool, ktory udava, ci premmenna je vo funkcii (true - vo funkcii; false - v maine)
*/
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

/**
 * @brief Funkcia na vygenerovanie zaciatku definicie funkcie
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
 * @param token Token obsahujuci nazov funkcie
*/
void gen_func(string_t *output, token_t *token){
    append_line(output, "LABEL ");
    append_line(output, token->attribute.str.data);
    append_line(output, "\nCREATEFRAME\n"
                        "PUSHFRAME\n"
                        "DEFVAR LF@tmp1\n");
}

/**
 * @brief Funckia na vygenerovanie konca definicie funkcie
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
*/
void gen_func_end(string_t *output){
    append_line(output, "POPFRAME\n"
                        "RETURN\n");
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

/**
 * @brief Funkcia na genrocanie zaciatku if konstrukcie
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
 * @param counter Pocitadlo konstrukcii
*/
void gen_if(string_t *output, int counter){
    append_line(output, "# body of if\n"
                        "POPS GF@tmp0\n"
                        "JUMPIFNEQ else");
    char str[16];
    sprintf(str, "%d\n", counter);
    append_line(output, str);
    append_line(output, "GF@tmp0 bool@true\n");
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
}

/**
 * @brief Funkcia na generovania konca if-else konstrukcie
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
 * @param counter Pocitadlo konstrukcii
*/
void gen_if_end(string_t *output, int counter){
    append_line(output, "LABEL if_end");
    char str[16];
    sprintf(str, "%d\n", counter);
    append_line(output, str);
}

/**
 * @brief Funckia na genrovanie zaciatku while konstrukcie
 * @param output Ukazatel na dynamicky string, v ktorom je output
 * @param counter Pocitadlo konstrukcii
*/
void gen_while(string_t *output, int counter){
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
    append_line(output, "POPS GF@tmp0\n"
                        "JUMPIFNEQ while_end");
    char str[16];
    sprintf(str, "%d", counter);
    append_line(output, str);
    append_line(output, "GF@tmp0 bool@true\n");
}

/**
 * @brief Funckia na generovanie konca while konstrukcie
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
 * @param counter Pocitadlo konstrukcii
*/
void gen_while_end(string_t *output, int counter){
    append_line(output, "LABEL while_end");
    char str[16];
    sprintf(str, "%d\n", counter);
    append_line(output, str);
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
                "LABEL readDoub\n"
                "CREATEFRAME\n"
                "PUSHFRAME\n"
                "DEFVAR LF@doub\n"
                "READ LF@doub float\n"
                "PUSHS LF@doub\n"
                "POPFRAME\n"
                "RETURN\n");
}

/**
 * @brief Funkcia na zadanie poctu argumentov, ktore sa maju vypisat
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
 * @param numberOfArguments Pocet argumentov, ktore sa maju vytlacit
*/
void gen_write_num_of_arg(string_t *output, int numberOfArguments){
    append_line(output, "PUSHS int@");
    char str[16];
    sprintf(str, "%d\n", numberOfArguments);
    append_line(output, str);
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
                        "DEFVAR LF@num\n"
                        "DEFVAR LF@arg\n"
                        "POPS LF@num\n"
                        "LABEL write_cyc\n"
                        "JUMPIFEQ write_end LF@num int@0\n"
                        "POPS LF@arg\n"
                        "WRITE LF@arg\n"
                        "SUB LF@num LF@num int@1\n"
                        "JUMP write_cyc\n"
                        "LABEL write_end\n"
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
                        "LABEL int2float\n"
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
                        "LABEL float2int\n"
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
                        "DEFVAR LF@tmp0\n"
                        "DEFVAR LF@tmp1\n"
                        "POPS LF@tmp0\n"
                        "STRLEN LF@tmp1 LF@tmp0\n"
                        "POPS LF@tmp1\n"
                        "POPFRAME\n"
                        "RETURN\n");
}

/**
 * @brief Funkcia na vytvorenie substringu
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
*/
void gen_substring(string_t *output){
    //todo
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
                        "DEFVAR LF@tmp0\n"
                        "DEFVAR LF@tmp1\n"
                        "POPS LF@tmp0\n"
                        "GETCHAR LF@tmp1 LF@tmp0 int@0\n"
                        );
                        //ako ziskam ordinalnu hondotu?
}

/**
 * @brief Funkcia na ziskanie prveho znaku v stringu
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
*/
void gen_chr(string_t *output){
    // ako?
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

/**
 * @brief Pomocna funkcia na zistenie nerovnosti
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
*/
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

/**
 * @brief Pomocna funkcia na zistenie, ci je lavy operand mensi ako pravy
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
*/
void gen_lesser(string_t *output){
    append_line(output, "# function lesser\n"
                        "LABEL lesser\n"
                        "POPS GF@tmp0\n"
                        "POPS GF@tmp1\n"
                        "LT GF@tmp2 GF@tmp0 GF@tmp1\n"
                        "PUSHS GF@tmp2\n"
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
                        "POPS GF@tmp0\n"
                        "POPS GF@tmp1\n"
                        "LT GF@tmp2 GF@tmp0 GF@tmp1\n"
                        "EQ GF@tmp3 GF@tmp0 GF@tmp1\n"
                        "PUSHS GF@tmp2\n"
                        "PUSHS GF@tmp3\n"
                        "ORS\n"
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
                        "POPS GF@tmp0\n"
                        "POPS GF@tmp1\n"
                        "PUSHS GF@tmp1\n"
                        "PUSHS GF@tmp0\n"
                        "CALL lesser\n"
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
                        "POPS GF@tmp0\n"
                        "POPS GF@tmp1\n"
                        "PUSHS GF@tmp1\n"
                        "PUSHS GF@tmp0\n"
                        "CALL lesser_or_equal\n"
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
                        "POPS GF@tmp0\n"
                        "POPS GF@tmp1\n"
                        "CONCAT GF@tmp2 GF@tmp0 GF@tmp1\n"
                        "PUSHS GF@tmp2\n"
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

/**
 * @brief Pomocna funkcia na pripravu hodnot pre dalsie spracovanie
 * 
 * @param output Ukazatel na dynamicky string, v ktorom je output
*/
void prepare_values(string_t *output){
    append_line(output, "# prepare values\n"
                        "LABEL prepare\n"
                        "POPS GF@tmp0\n"
                        "TYPE GF@type0\n"
                        "JUMPIFEQ prep_string GF@type0 string@string\n"
                        "JUMPIFEQ prep_float GF@type0 string@string\n");
}