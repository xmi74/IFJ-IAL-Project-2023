/**
 * IFJ-IAL Projekt 2023
 * 
 * @file parser.h
 * @brief Implementacia programu syntakticke a semanticke analyzy
 * 
 * @author Igor Mikula (xmikul74)
 * @author Marko Olesak (xolesa00)
 * @author Jan Findra (xfindr01)
 * @author Tomas Arlt (xarltt00)
*/

#include <stdbool.h>
#include "symtable.c"


//typedef enum keyword_type
//{
//    kw_id,
//    kw_Double,
//    kw_else,
//    kw_func,
//    kw_if,
//    kw_Int,
//    kw_let,
//    kw_nil,
//    kw_return,
//    kw_String,
//    kw_var,
//    kw_while
//} keyword_t;
//
//keyword_t keyword_type(token_t token);

typedef struct func_table_member
{
    string_t key;
    type_t type;
    token_t params[100];
} func_table_member_t;

typedef struct func_table
{
    func_table_member_t *members;
    int size;
    int capacity;
} func_table_t;

int handle_assignment(token_t token_assigner, global_symtab_t *global_table, local_symtab_w_par_ptr_t *local_table);

void read_subblock(token_t token);

int find_functions();

int parse_block(int nest_level, global_symtab_t *global_table, local_symtab_w_par_ptr_t *local_table_one_up);

int parse();
