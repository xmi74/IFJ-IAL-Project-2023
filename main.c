#include "parser.h"
#include "code_gen.h"
#include <assert.h>

token_table_t token_table;
string_t *output;
int counter = 0;

int main()
{
    output = gen_start();
    parse();
    gen_end(output);
}