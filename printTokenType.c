#include "scanner.h"

const char *getTokenTypeName(token_type_t tokenType)
{
    switch (tokenType)
    {
    case TOK_NOTHING:
        return "TOK_NOTHING";
    case TOK_IDENTIFIER:
        return "TOK_IDENTIFIER";
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
    case TOK_DOUBLE_QUEST_MARK:
        return "TOK_DOUBLE_QUEST_MARK";
    case TOK_COMMENT:
        return "TOK_COMMENT";
    case TOK_BLOCK_COM_START:
        return "TOK_BLOCK_COM_START";
    case TOK_BLOCK_COM_END:
        return "TOK_BLOCK_COM_END";
    case TOK_KW_NIL:
        return "TOK_KW_NIL";
    default:
        return "Unknown token type";
    }
}