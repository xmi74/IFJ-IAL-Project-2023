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
    newNode->type = token.type;
    newNode->token = token;
    newNode->left = NULL;
    newNode->right = NULL;

    return newNode;
}

/**
 * @brief Funkcia na kontrolu typov.
 * @param root Ukazatel na koren stromu.
 */
void checkLeafTypes(ast_node_t *root, bool doubleQuestMark)
{
    // Nemalo by nastat
    if (root->left == NULL && root->right == NULL)
    {
        root->type = TOK_NOTHING;
        fprintf(stderr, "[EXPR] Father node has no children");
        returnError(INTERN_ERR);
        return;
    }

    if (root->left != NULL && root->right != NULL)
    {
        token_type_t leftType = root->left->type;
        token_type_t rightType = root->right->type;

        // Urcenie vysledneho typu pri operatore '??' s TOK_KW_NIL
        if (doubleQuestMark &&
            ((leftType == TOK_KW_NIL && rightType != TOK_KW_NIL) ||
             (rightType == TOK_KW_NIL && leftType != TOK_KW_NIL)))
        {
            root->type = (leftType == TOK_KW_NIL) ? rightType : leftType;
            return;
        }
        bool leftLiteral = root->left->literal;
        bool rightLiteral = root->right->literal;

        // Ponechanie informacii o literale korenu
        root->literal = leftLiteral && rightLiteral;

        // Kontrola pretypovania, kde INT musi byt literal
        if (leftType == rightType ||
            (leftType == TOK_DOUBLE && rightLiteral && rightType == TOK_INT) ||
            (leftLiteral && leftType == TOK_INT && rightType == TOK_DOUBLE))
        {
            root->type = (leftType == TOK_INT && rightType == TOK_DOUBLE) ? TOK_DOUBLE : leftType;
            return;
        }

        // Kontrola operandov uz bola vykonana
        if (root->type == TOK_EQUAL || root->type == TOK_NOT_EQUAL){
            if (leftType == TOK_KW_NIL || rightType == TOK_KW_NIL || root->left->token.attribute.includesNil == true || root->right->token.attribute.includesNil == true) {
                return;
            }
        }

        // If none of the above conditions met, there's a type error
        root->type = TOK_NOTHING;
        fprintf(stderr, "[EXPR] Type compatibility error");
        returnError(TYPE_COMPATIBILITY_ERR);
    }
    else
    {
        // Otec ma jednoho syna
        root->type = (root->left != NULL) ? root->left->type : root->right->type;
        root->literal = (root->left != NULL) ? root->left->literal : root->right->literal;
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
ast_node_t *make_tree(token_t fatherToken, ast_node_t *left, ast_node_t *right, bool doubleQuestMark)
{
    ast_node_t *fatherNode = make_leaf(fatherToken);
    fatherNode->left = left;
    fatherNode->right = right;
    checkLeafTypes(fatherNode, doubleQuestMark);

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
