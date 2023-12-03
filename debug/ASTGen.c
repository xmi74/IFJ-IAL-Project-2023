#include <stdio.h>
#include <stdlib.h>
#include "../abstract_syntax_tree.h"

const char *tokenTypeToString(token_type_t type)
{
    switch (type)
    {
    case TOK_NOTHING:
        return "TOK_NOTHING";
    case TOK_IDENTIFIER:
        return "TOK_IDENTIFIER";
    case TOK_KW_DOUBLE:
        return "TOK_KW_DOUBLE";
    case TOK_KW_ELSE:
        return "TOK_KW_ELSE";
    case TOK_KW_FUNC:
        return "TOK_KW_FUNC";
    case TOK_KW_IF:
        return "TOK_KW_IF";
    case TOK_KW_INT:
        return "TOK_KW_INT";
    case TOK_KW_LET:
        return "TOK_KW_LET";
    case TOK_KW_NIL:
        return "TOK_KW_NIL";
    case TOK_KW_RETURN:
        return "TOK_KW_RETURN";
    case TOK_KW_STRING:
        return "TOK_KW_STRING";
    case TOK_KW_VAR:
        return "TOK_KW_VAR";
    case TOK_KW_WHILE:
        return "TOK_KW_WHILE";
    case TOK_UNDERSCORE:
        return "TOK_UNDERSCORE";
    case TOK_EOF:
        return "TOK_EOF";
    case TOK_INT:
        return "TOK_INT";
    case TOK_DOUBLE:
        return "TOK_DOUBLE";
    case TOK_STRING_START:
        return "TOK_STRING_START";
    case TOK_STRING:
        return "TOK_STRING";
    case TOK_MUL:
        return "TOK_MUL";
    case TOK_DIV:
        return "TOK_DIV";
    case TOK_PLUS:
        return "TOK_PLUS";
    case TOK_MINUS:
        return "TOK_MINUS";
    case TOK_R_CRL_BRCKT:
        return "TOK_R_CRL_BRCKT";
    case TOK_L_CRL_BRCKT:
        return "TOK_L_CRL_BRCKT";
    case TOK_R_BRCKT:
        return "TOK_R_BRCKT";
    case TOK_L_BRCKT:
        return "TOK_L_BRCKT";
    case TOK_ARROW:
        return "TOK_ARROW";
    case TOK_NOT:
        return "TOK_NOT";
    case TOK_NOT_EQUAL:
        return "TOK_NOT_EQUAL";
    case TOK_LESSER:
        return "TOK_LESSER";
    case TOK_LESSER_OR_EQUAL:
        return "TOK_LESSER_OR_EQUAL";
    case TOK_GREATER:
        return "TOK_GREATER";
    case TOK_GREATER_OR_EQUAL:
        return "TOK_GREATER_OR_EQUAL";
    case TOK_EQUAL:
        return "TOK_EQUAL";
    case TOK_ASSIGN:
        return "TOK_ASSIGN";
    case TOK_COLON:
        return "TOK_COLON";
    case TOK_COMMA:
        return "TOK_COMMA";
    case TOK_SEMICLN:
        return "TOK_SEMICLN";
    case TOK_DOT:
        return "TOK_DOT";
    case TOK_QUEST_MARK:
        return "TOK_QUEST_MARK";
    case TOK_DOUBLE_QUEST_MARK:
        return "TOK_DOUBLE_QUEST_MARK";
    case TOK_COMMENT:
        return "TOK_COMMENT";
    case TOK_BLOCK_COM_START:
        return "TOK_BLOCK_COM_START";
    case TOK_BLOCK_COM_END:
        return "TOK_BLOCK_COM_END";
    case TOK_EOL:
        return "TOK_EOL";
    default:
        return "UNKNOWN";
    }
}

void generateAST(FILE *dotFile, ast_node_t *tree)
{
    if (tree != NULL)
    {
        if (tree->token.type == TOK_PLUS)
        {
            fprintf(dotFile, "  node%p [label=\"+ (%s)(N:%s)\"];\n", (void *)tree, tokenTypeToString(tree->type), tree->token.attribute.includesNil ? "true" : "false");
        }
        else if (tree->token.type == TOK_MINUS)
        {
            fprintf(dotFile, "  node%p [label=\"- (%s)(N:%s)\"];\n", (void *)tree, tokenTypeToString(tree->type), tree->token.attribute.includesNil ? "true" : "false");
        }
        else if (tree->token.type == TOK_MUL)
        {
            fprintf(dotFile, "  node%p [label=\"* (%s)(N:%s)\"];\n", (void *)tree, tokenTypeToString(tree->type), tree->token.attribute.includesNil ? "true" : "false");
        }
        else if (tree->token.type == TOK_DIV)
        {
            fprintf(dotFile, "  node%p [label=\"/ (%s)(N:%s)\"];\n", (void *)tree, tokenTypeToString(tree->type), tree->token.attribute.includesNil ? "true" : "false");
        }
        else if (tree->token.type == TOK_NOT_EQUAL)
        {
            fprintf(dotFile, "  node%p [label=\"!= (%s)(N:%s)\"];\n", (void *)tree, tokenTypeToString(tree->type), tree->token.attribute.includesNil ? "true" : "false");
        }
        else if (tree->token.type == TOK_LESSER)
        {
            fprintf(dotFile, "  node%p [label=\"< (%s)(N:%s)\"];\n", (void *)tree, tokenTypeToString(tree->type), tree->token.attribute.includesNil ? "true" : "false");
        }
        else if (tree->token.type == TOK_LESSER_OR_EQUAL)
        {
            fprintf(dotFile, "  node%p [label=\"<= (%s)(N:%s)\"];\n", (void *)tree, tokenTypeToString(tree->type), tree->token.attribute.includesNil ? "true" : "false");
        }
        else if (tree->token.type == TOK_GREATER)
        {
            fprintf(dotFile, "  node%p [label=\"> (%s)(N:%s)\"];\n", (void *)tree, tokenTypeToString(tree->type), tree->token.attribute.includesNil ? "true" : "false");
        }
        else if (tree->token.type == TOK_GREATER_OR_EQUAL)
        {
            fprintf(dotFile, "  node%p [label=\">= (%s)(N:%s)\"];\n", (void *)tree, tokenTypeToString(tree->type), tree->token.attribute.includesNil ? "true" : "false");
        }
        else if (tree->token.type == TOK_EQUAL)
        {
            fprintf(dotFile, "  node%p [label=\"== (%s)(N:%s)\"];\n", (void *)tree, tokenTypeToString(tree->type), tree->token.attribute.includesNil ? "true" : "false");
        }
        else if (tree->token.type == TOK_ASSIGN)
        {
            fprintf(dotFile, "  node%p [label=\"= (%s)(N:%s)\"];\n", (void *)tree, tokenTypeToString(tree->type), tree->token.attribute.includesNil ? "true" : "false");
        }
        else if (tree->token.type == TOK_IDENTIFIER)
        {
            fprintf(dotFile, "  node%p [label=\"%s (%s)(N:%s)\"];\n", (void *)tree, tree->token.attribute.str.data, tokenTypeToString(tree->type), tree->token.attribute.includesNil ? "true" : "false");
        }
        else if (tree->token.type == TOK_INT)
        {
            fprintf(dotFile, "  node%p [label=\"%d (%s)(N:%s)\"];\n", (void *)tree, tree->token.attribute.intValue, tokenTypeToString(tree->type), tree->token.attribute.includesNil ? "true" : "false");
        }
        else if (tree->token.type == TOK_DOUBLE)
        {
            fprintf(dotFile, "  node%p [label=\"%f (%s)(N:%s)\"];\n", (void *)tree, tree->token.attribute.doubleValue, tokenTypeToString(tree->type), tree->token.attribute.includesNil ? "true" : "false");
        }
        else if (tree->token.type == TOK_STRING)
        {
            fprintf(dotFile, "  node%p [label=\"STRING (%s)(N:%s)\"];\n", (void *)tree, tokenTypeToString(tree->type), tree->token.attribute.includesNil ? "true" : "false");
        }
        else if (tree->token.type == TOK_NOT)
        {
            fprintf(dotFile, "  node%p [label=\"! (%s)(N:%s)\"];\n", (void *)tree, tokenTypeToString(tree->type), tree->token.attribute.includesNil ? "true" : "false");
        }
        else if (tree->token.type == TOK_DOUBLE_QUEST_MARK)
        {
            fprintf(dotFile, "  node%p [label=\"?? (%s)(N:%s)\"];\n", (void *)tree, tokenTypeToString(tree->type), tree->token.attribute.includesNil ? "true" : "false");
        }
        else if (tree->token.type == TOK_KW_NIL)
        {
            fprintf(dotFile, "  node%p [label=\"?? (%s)(N:%s)\"];\n", (void *)tree, tokenTypeToString(tree->type), tree->token.attribute.includesNil ? "true" : "false");
        }

        if (tree->left != NULL)
        {
            fprintf(dotFile, "  node%p -> node%p [label=\"left\"];\n", (void *)tree, (void *)(tree->left));
            generateAST(dotFile, tree->left);
        }

        if (tree->right != NULL)
        {
            fprintf(dotFile, "  node%p -> node%p [label=\"right\"];\n", (void *)tree, (void *)(tree->right));
            generateAST(dotFile, tree->right);
        }
    }
}

void createFileAST(ast_node_t *tree)
{
    FILE *dotFile = fopen("ast.dot", "w");
    if (dotFile == NULL)
    {
        perror("Subor ast.dot nemozno otvorit\n");
        return;
    }

    fprintf(dotFile, "digraph AST {\n");
    generateAST(dotFile, tree);
    fprintf(dotFile, "}\n");
    fclose(dotFile);
}

void ast_gen(ast_node_t *tree)
{
    createFileAST(tree);
    system("dot -Tpng ast.dot -o ast.png");
}