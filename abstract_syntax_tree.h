/**
 * IFJ-IAL Projekt 2023
 *
 * @file abstract_syntax_tree.h
 * @brief Abstraktny syntakticky strom
 *
 * @author Igor Mikula (xmikul74)
 * @author Marko Olesak (xolesa00)
 * @author Jan Findra (xfindr01)
 * @author Tomas Arlt (xarltt00)
 */


#ifndef ABSTRACT_SYNTAX_TREE_H
#define ABSTRACT_SYNTAX_TREE_H

#include <stdio.h>
#include <stdlib.h>
#include "scanner.h"
#include "error.h"
typedef struct ast_node
{
    token_t token;
    struct ast_node *left;
    struct ast_node *right;
    token_type_t type;
    bool literal;
} ast_node_t;

typedef struct ast_items
{
    ast_node_t **nodes;
    int capacity;
    int size;
} ast_items_t;

// Prototypy funkcii
void ast_init(ast_node_t *tree);

ast_node_t *make_leaf(token_t token);

void checkLeafTypes(ast_node_t *root, bool doubleQuestMark);

ast_node_t *make_tree(token_t fatherToken, ast_node_t *left, ast_node_t *right, bool doubleQuestMark);

void ast_dispose(ast_node_t *tree);

void items_init(ast_items_t *items);

void ast_add_node_to_items(ast_node_t *node, ast_items_t *items);

void ast_postorder(ast_node_t *tree, ast_items_t *items);

#endif