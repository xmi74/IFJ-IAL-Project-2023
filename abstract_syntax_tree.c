/**
 * IFJ-IAL Projekt 2023
 *
 * @file error.h
 * @brief Abstrakcny syntakticky strom
 *
 * @author Igor Mikula (xmikul74)
 * @author Marko Olesak (xolesa00)
 * @author Jan Findra (xfindr01)
 * @author Tomas Arlt (xarltt00)
 */

#include "abstract_syntax_tree.h"

/**
 * @brief Funkcia na inicializovanie AST.
 *
 * @param tree Ukazatel na strom.
 */
void ast_init(ast_node_t *tree)
{
    tree = malloc(sizeof(ast_node_t));

    if (tree == NULL)
    {
        returnError(99);
    }
}

/**
 * @brief Funkcia na vlozenie listu do AST.
 *
 * @param token Token noveho uzlu.
 * @return Ukazatel na novy uzol.
 */

ast_node_t *make_leaf(token_t token)
{
    ast_node_t *newNode = malloc(sizeof(ast_node_t));

    if (newNode == NULL)
    {
        returnError(99);
    }
    newNode->token = token;
    newNode->left = NULL;
    newNode->right = NULL;

    return newNode;
}

/**
 * @brief Funkcia na kontrolu typov.
 * @param root Ukazatel na koren stromu.
 */
void checkTypes(ast_node_t *root)
{
    // skontroluj typy laveho a praveho podstromu, podla toho nastav typ otca
    if (root->left != NULL && root->right != NULL)
    {
        // Ak su typy rovnake, nastav typ otca na ten isty
        if (root->left->type == root->right->type)
        {
            root->type = root->left->type;
        }

        // INT + Double, kde INT musi byt LITERAL!
        else if (root->left->type == TOK_DOUBLE && (root->right->type == TOK_INT && root->right->token.type == TOK_INT))
        {
            root->type = TOK_DOUBLE;
        }
        else if ((root->left->token.type == TOK_INT && root->left->type == TOK_INT) && root->right->type == TOK_DOUBLE)
        {
            root->type = TOK_DOUBLE;
        }
        else
        {
            root->type = TOK_NOTHING; // chyba typov, nemalo by nastat, kontroluje sa uz v expr
returnError(TYPE_COMPATIBILITY_ERR);
        }
    }

    else if (root->left != NULL)
    {
        root->type = root->left->type;
    }
    else if (root->right != NULL)
    {
        root->type = root->right->type;
    }
    else
    {
        root->type = TOK_EOF;
    }

    // Zachovanie informacie o literale
    if (root->left->literal == true && root->right->literal == true)
    {
        root->literal = true;
    }
    else
    {
        root->literal = false;
    }
}

/**
 * @brief Funkcia na vytvorenie stromu z uzlov.
 *
 * @param fatherToken Token korena (otca) stromu.
 * @param left Ukazatel na lavy uzol.
 * @param right Ukazatel na pravy uzol.
 * @return Ukazatel na koren (otca) stromu.
 */
ast_node_t *make_tree(token_t fatherToken, ast_node_t *left, ast_node_t *right)
{
    ast_node_t *fatherNode = make_leaf(fatherToken);
    fatherNode->left = left;
    fatherNode->right = right;
    checkTypes(fatherNode);

    return fatherNode;
}

/**
 * @brief Funkcia na zrusenie AST a uvolnenie alokovanych prostriedkov.
 *
 * @param tree Ukazatel na koren stromu.
 */

void ast_dispose(ast_node_t *tree)
{
    if (tree != NULL)
    {
        ast_dispose(tree->left);
        ast_dispose(tree->right);
        freeToken(&(tree->token)); // uvolnenie aj pamati hodnoty
        free(tree);
    }
    tree = NULL;
}

/**
 * @brief Funkcia na inicializovanie pola uzlov AST pre postorder.
 *
 * @param items Ukazatel na strukturu.
 */

void items_init(ast_items_t *items)
{
    items->capacity = 0;
    items->nodes = NULL;
    items->size = 0;
}

/**
 * @brief Funkcia na vlozenie noveho uzla do pola.
 *
 * @param node Ukazatel na uzol, ktory ma byt vlozeny.
 * @param items Ukazatel na strukturu, do ktorej ma byt uzol vlozeny.
 */

void ast_add_node_to_items(ast_node_t *node, ast_items_t *items)
{
    if (items->capacity < items->size + 1)
    {
        items->capacity = items->capacity * 2 + 8;
        items->nodes = realloc(items->nodes, items->capacity * (sizeof(ast_node_t *)));
    }
    items->nodes[items->size] = node;
    items->size++;
}

/**
 * @brief Funkcia na prejdenie AST v postorderi.
 *
 * @param tree Ukazatel na koren AST.
 * @param items Struktura, do ktorej ma byt postorder ulozeny.
 */

void ast_postorder(ast_node_t *tree, ast_items_t *items)
{
    if (tree != NULL)
    {
        ast_postorder(tree->left, items);
        ast_postorder(tree->right, items);
        ast_add_node_to_items(tree, items);
    }
}
