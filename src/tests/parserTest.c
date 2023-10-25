#include "../parser.c"
#include <assert.h>

int main()
{
    //parse();
    local_symtab_w_par_ptr_t local_table;
    local_init_w_par_ptr_t(&local_table);
    printf("local_table.table: %p\n", local_table.table);
    printf("local_table.parent: %p\n", local_table.parent);
}