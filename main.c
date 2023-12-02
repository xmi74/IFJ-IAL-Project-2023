#include "parser.h"
#include "code_gen.h"
#include <assert.h>

token_table_t token_table;
string_t *output;
string_t *localVariables;
int counter = 0;
int nestLevel = 0;

int main()
{
    output = gen_start();
    parse();
    gen_end(output);
}