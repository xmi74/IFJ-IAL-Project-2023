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

#include "symtable.c"


typedef enum keyword_type
{
    kw_id,
    kw_Double,
    kw_else,
    kw_func,
    kw_if,
    kw_Int,
    kw_let,
    kw_nil,
    kw_return,
    kw_String,
    kw_var,
    kw_while
} keyword_t;

keyword_t keyword_type(token_t token);

int handle_assignment(token_t token_assigner, global_symtab_t *global_table, local_symtab_t *local_table);

int parse_block(int nest_level, global_symtab_t *global_table, local_symtab_t *local_table_one_up);

int parse();
