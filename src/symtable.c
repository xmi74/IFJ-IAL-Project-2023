/**
 * IFJ-IAL Projekt 2023
 * 
 * @file symtable.c
 * @brief Implementacia tabulky symbolov (identifikatorov) 
 * 
 * @author Igor Mikula (xmikul74)
 * @author Marko Olesak (xolesa00)
 * @author Jan Findra (xfindr01)
 * @author Tomas Arlt (xarltt00)
*/

#include "symtable.h"

/****************************************************/
/*               LOKALNA TABULKA SYMB.              */
/****************************************************/

int max(int n1, int n2)
{
    if (n1 > n2)
    {
        return n1;
    }
    else
    {
        return n2;
    }
}

int heightL(local_symtab_t *ptr)
{
    if (ptr != NULL)
    {
        return max(heightL(ptr->left), heightL(ptr->right)) + 1;
    }
    else
    {
        return 0;
    }
}

int balanceL(local_symtab_t *local_table)
{
    if (local_table != NULL)
    {
        return heightL(local_table->left) - heightL(local_table->right);
    }
    else
        return 0;
}


local_symtab_t* rotateRightL(local_symtab_t *criticalNode)  // y = critical node, x = leftChild
{
    local_symtab_t *leftChild = criticalNode->left;
    criticalNode->left = leftChild->right;
    leftChild->right = criticalNode;

    criticalNode->height = max(heightL(criticalNode->left), heightL(criticalNode->right)) + 1;
    leftChild->height = max(heightL(leftChild->left), heightL(leftChild->right)) + 1;

    return leftChild;
}

local_symtab_t* rotateLeftL(local_symtab_t *criticalNode) // y = critical node, x = rightChild
{
    local_symtab_t *rightChild = criticalNode->right;
    criticalNode->right = rightChild->left;
    rightChild->left = criticalNode;

    criticalNode->height = max(heightL(criticalNode->left), heightL(criticalNode->right)) + 1;
    rightChild->height = max(heightL(rightChild->left), heightL(rightChild->right)) + 1;

    return rightChild;
}

void local_init(local_symtab_t **local_table)
{
    (*local_table) = NULL;
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

local_symtab_t* local_insert(local_symtab_t *local_table, string_t *key, token_type_t type)
{
    
    // Aktualizacna semantika vkladania prvku
    if (local_table == NULL)
    {
        // pridanie noveho uzlu
        local_table = (local_symtab_t *)malloc(sizeof(struct local_symtab));
        dstringInit(&(local_table->key));
        dstringCopy(&(local_table->key), key);
        local_table->type = type;
        local_table->left = NULL;
        local_table->right = NULL;
        local_table->height = 1;
    }
    else
    {               
        int cmp = dstringCompare(key, &(local_table->key));        
        if (cmp == 0)
        {
            return local_table;
        }
        else if (cmp < 0)
        {
            local_table->left = local_insert(local_table->left, key, type);
        }
        else // if (cmp > 0)
        {            
            local_table->right = local_insert(local_table->right, key, type);
        }

        // Vyskove vyvazenie
        local_table->height = 1 + max(heightL(local_table->left), heightL(local_table->right));
        int balanceFactor = balanceL(local_table);
        
        // LL -> oprava pravou rotaciou
        if (balanceFactor > 1 && dstringCompare(key, &(local_table->left->key)) < 0)
        {
            return rotateRightL(local_table);
        }
        // RR -> oprava lavou rotaciou
        if (balanceFactor < -1 && dstringCompare(key, &(local_table->right->key)) > 0)
        {
            return rotateLeftL(local_table);
        }
        // LR -> oprava lavou a nasledne pravou rotaciou
        if (balanceFactor > 1 && dstringCompare(key, &(local_table->left->key)) > 0)
        {
            local_table->left = rotateLeftL(local_table->left);
            return rotateRightL(local_table);
        }
        // RL -> oprava pravou a nasledne lavou rotaciou
        if (balanceFactor < -1 && dstringCompare(key, &(local_table->right->key)) < 0)
        {
            local_table->right = rotateRightL(local_table->right);
            return rotateLeftL(local_table);
        }
    }

    return local_table;
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

int heightG(global_symtab_t *ptr)
{
    if (ptr != NULL)
    {
        return max(heightG(ptr->left), heightG(ptr->right)) + 1;
    }
    else
    {
        return 0;
    }
}

int balanceG(global_symtab_t *global_table)
{
    if (global_table != NULL)
    {
        return heightG(global_table->left) - heightG(global_table->right);
    }
    else
        return 0;
}


global_symtab_t* rotateRightG(global_symtab_t *criticalNode)  // y = critical node, x = leftChild
{
    global_symtab_t *leftChild = criticalNode->left;
    criticalNode->left = leftChild->right;
    leftChild->right = criticalNode;

    criticalNode->height = max(heightG(criticalNode->left), heightG(criticalNode->right)) + 1;
    leftChild->height = max(heightG(leftChild->left), heightG(leftChild->right)) + 1;

    return leftChild;
}

global_symtab_t* rotateLeftG(global_symtab_t *criticalNode) // y = critical node, x = rightChild
{
    global_symtab_t *rightChild = criticalNode->right;
    criticalNode->right = rightChild->left;
    rightChild->left = criticalNode;

    criticalNode->height = max(heightG(criticalNode->left), heightG(criticalNode->right)) + 1;
    rightChild->height = max(heightG(rightChild->left), heightG(rightChild->right)) + 1;

    return rightChild;
}

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


global_symtab_t* global_insert(global_symtab_t *global_table, string_t *key, type_t type, string_t params)
{
    if (global_table == NULL)
    {        
        global_table = (global_symtab_t *)malloc(sizeof(struct global_symtab));
        dstringInit(&(global_table->key));
        dstringCopy(&(global_table->key), key);
        global_table->type = type;
        global_table->params = params;
        global_table->left = NULL;
        global_table->right = NULL;
        global_table->height = 1;
    }
    else
    {
        int cmp = dstringCompare(key, &(global_table->key));        
        if (cmp == 0)
        {
            return global_table;
        }
        else if (cmp < 0)
        {            
            global_insert(global_table->left, key, type, params);
        }
        else
        {            
            global_insert(global_table->right, key, type, params);
        }

        // Vyskove vyvazenie
        global_table->height = 1 + max(heightG(global_table->left), heightG(global_table->right));
        int balanceFactor = balanceG(global_table);
        
        // LL -> oprava pravou rotaciou
        if (balanceFactor > 1 && dstringCompare(key, &(global_table->left->key)) < 0)
        {
            return rotateRightG(global_table);
        }
        // RR -> oprava lavou rotaciou
        if (balanceFactor < -1 && dstringCompare(key, &(global_table->right->key)) > 0)
        {
            return rotateLeftG(global_table);
        }
        // LR -> oprava lavou a nasledne pravou rotaciou
        if (balanceFactor > 1 && dstringCompare(key, &(global_table->left->key)) > 0)
        {
            global_table->left = rotateLeftG(global_table->left);
            return rotateRightG(global_table);
        }
        // RL -> oprava pravou a nasledne lavou rotaciou
        if (balanceFactor < -1 && dstringCompare(key, &(global_table->right->key)) < 0)
        {
            global_table->right = rotateRightG(global_table->right);
            return rotateLeftG(global_table);
        }
    }
    return global_table;
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
