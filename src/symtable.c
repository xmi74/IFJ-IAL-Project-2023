/**
 * IFJ-IAL Projekt 2023
 * 
 * @file symtable.c
 * @brief Implementacia tabulky symbolov (identifikatorov) 
 * 
 * @author Igor Mikula (xmikul74)
 * @author Marko Olesak (xolesa00)
 * @author Jan Findra (xfindr01)
 * @author
*/

#include "symtable.h"

/****************************************************/
/*               LOKALNA TABULKA SYMB.              */
/****************************************************/

void local_init(local_symtab_t **local_table)
{
    (*local_table) = NULL;
}

void local_init_w_par_ptr_t(local_symtab_w_par_ptr_t *local_table)
{
    local_init(&(local_table->table));
    local_table->parent = NULL;
}

local_symtab_t *local_search(local_symtab_t *local_table, string_t *key)
{
    if (local_table != NULL)
    {
        int cmp = dstringCompare(key, &(local_table->key));
        if (cmp == 0)
        {
            return local_table;
        }
        if (cmp < 0)
        {
            return local_search(local_table->left, key);
        }
        if (cmp > 0)
        {
            return local_search(local_table->right, key);
        }
    }
    return NULL;
}

local_symtab_t *local_search_in_all(local_symtab_w_par_ptr_t *local_table, string_t *key)
{
    local_symtab_w_par_ptr_t *tmp = local_table;
    while (tmp != NULL)
    {
        local_symtab_t *found = local_search(tmp->table, key);
        if (found != NULL)
        {
            return found;
        }
        tmp = tmp->parent;
    }
    return NULL;
}

void local_insert(local_symtab_t **local_table, string_t *key, token_type_t type)
{
    if (*local_table == NULL)
    {
        // pridanie noveho uzlu
        *local_table = (local_symtab_t *)malloc(sizeof(struct local_symtab));
        dstringInit(&((*local_table)->key));
        dstringCopy(&((*local_table)->key), key);
        (*local_table)->type = type;
        (*local_table)->left = NULL;
        (*local_table)->right = NULL;
        
    }
    else
    {               
        int cmp = dstringCompare(key, &((*local_table)->key));        
        if (cmp == 0)
        {
            return;
        }
        if (cmp < 0)
        {            
            local_insert(&((*local_table)->left), key, type);
        }
        if (cmp > 0)
        {            
            local_insert(&((*local_table)->right), key, type);
        }
    }
}

void local_dispose(local_symtab_t **local_table)
{
    if ((*local_table) != NULL)
    {
        local_dispose(&(*local_table)->left);
        local_dispose(&(*local_table)->right);

        dstringFree(&((*local_table)->key));
        free(*local_table);
    }
    (*local_table) = NULL;
}

/****************************************************/
/*               GLOBALNA TABULKA SYMB.             */
/****************************************************/

void global_init(global_symtab_t **global_table)
{
    (*global_table) = NULL;
}

global_symtab_t *global_search(global_symtab_t *global_table, string_t *key)
{
    if (global_table != NULL)
    {
        int cmp = dstringCompare(key, &(global_table->key));
        if (cmp == 0)
        {
            return global_table;
        }
        if (cmp < 0)
        {
            return global_search(global_table->left, key);
        }
        if (cmp > 0)
        {
            return global_search(global_table->right, key);
        }
    }
    return NULL;
}


void global_insert(global_symtab_t **global_table, string_t *key, type_t type, string_t params)
{
    if (*global_table == NULL)
    {        
        *global_table = (global_symtab_t *)malloc(sizeof(struct global_symtab));
        dstringInit(&((*global_table)->key));
        dstringCopy(&((*global_table)->key), key);
        (*global_table)->type = type;
        (*global_table)->params = params;
        (*global_table)->left = NULL;
        (*global_table)->right = NULL;
    }
    else
    {
        int cmp = dstringCompare(key, &((*global_table)->key));        
        if (cmp == 0)
        {
            return;
        }
        if (cmp < 0)
        {            
            global_insert(&((*global_table)->left), key, type, params);
        }
        if (cmp > 0)
        {            
            global_insert(&((*global_table)->right), key, type, params);
        }
    }
}

void global_dispose(global_symtab_t **global_table)
{
    if ((*global_table) != NULL)
    {
        global_dispose(&(*global_table)->left);
        global_dispose(&(*global_table)->right);

        dstringFree(&((*global_table)->key));
        free(*global_table);
    }
    (*global_table) = NULL;
}
