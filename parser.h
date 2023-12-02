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
#include "symtable.h"
#include "expr.h"
#include "code_gen.h"

token_type_t kw_to_token_type(token_type_t kw);

token_type_t type_t_to_token_type_t(type_t type);

type_t token_type_t_to_type_t(token_type_t type);

void load_built_in_functions();

void call_func(global_symtab_t *func, local_symtab_w_par_ptr_t *local_table, global_symtab_t *global_table, int nest_level);

void handle_variable(token_t token_assigner, global_symtab_t *global_table, local_symtab_w_par_ptr_t *local_table, int nest_level);

void handle_assign_or_call_func(token_t token_id, global_symtab_t *global_table, local_symtab_w_par_ptr_t *local_table, int nest_level);

void handle_func_def(global_symtab_t *global_table, local_symtab_w_par_ptr_t *local_table_one_up);

void handle_cond(local_symtab_w_par_ptr_t *local_table, global_symtab_t *global_table);

bool handle_if(int nest_level, local_symtab_w_par_ptr_t *local_table, global_symtab_t *global_table);

void handle_while(int nest_level, local_symtab_w_par_ptr_t *local_table, global_symtab_t *global_table);

void read_subblock(token_t token);

void find_functions(global_symtab_t **global_table);

bool parse_block(int nest_level, token_type_t block_start, global_symtab_t *global_table, local_symtab_w_par_ptr_t *local_table_one_up);

int parse();
