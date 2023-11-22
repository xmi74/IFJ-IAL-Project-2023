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


// Struktura tabulky tokenu
typedef struct
{
    token_t *tokens;
    int size;
    int capacity;
    int index;
    bool insert; // vkladani/cteni
} token_table_t;

// Inicializace tabulky tokenu
void initTokenTable(token_table_t *table);

// Uvolneni pameti tabulky tokenu
void freeTokenTable(token_table_t *table);

// Pridani tokenu do tabulky tokenu
void addTableToken(token_table_t *table, token_t token);

// Vraceni tokenu z tabulky tokenu
token_t getTableToken(token_table_t *table);

token_t getToken();

token_t getTokenAssert(token_type_t type);

int call_func(global_symtab_t *func, local_symtab_w_par_ptr_t *local_table, global_symtab_t *global_table);

int handle_variable(token_t token_assigner, global_symtab_t *global_table, local_symtab_w_par_ptr_t *local_table);

int handle_assign_or_call_func(token_t token_id, global_symtab_t *global_table, local_symtab_w_par_ptr_t *local_table);

int handle_func_def(global_symtab_t *global_table, local_symtab_w_par_ptr_t *local_table_one_up);

void read_subblock(token_t token);

void find_functions(global_symtab_t **global_table);

token_t parse_block(int nest_level, token_type_t block_start, global_symtab_t *global_table, local_symtab_w_par_ptr_t *local_table_one_up);

int parse();
