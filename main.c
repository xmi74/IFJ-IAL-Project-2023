/**
 * IFJ-IAL Projekt 2023
 * 
 * @file main.c
 * @brief Hlavny subor programu
 * 
 * @author Igor Mikula (xmikul74)
 * @author Marko Olesak (xolesa00)
 * @author Jan Findra (xfindr01)
 * @author Tomas Arlt (xarltt00)
*/

#include "parser.h"
#include "code_gen.h"
#include <assert.h>

token_table_t token_table;
string_t *output;
string_t *localVariables;
int global_counter = 0;
int local_counter = 0;
Stack counter_stack;
int nestLevel = 0;

int main()
{
    Stack_Init(&counter_stack);
    output = gen_start();
    parse();
    gen_end(output);
    Stack_Dispose(&counter_stack);
}