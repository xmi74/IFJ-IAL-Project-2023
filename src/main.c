#include <stdio.h>

#include "symtable.h"

//local_symtab_t* symbolTable = NULL;

// Funkcia pre graficke znazornenie tabulky symbolov
void generateGraphvizCode(local_symtab_t* root, FILE* file) 
{
    if (root != NULL) 
    {
        if (root->left != NULL) 
        {
            fprintf(file, "\"%s\" -> \"%s\" [label=\"L\"];\n", root->key.data, root->left->key.data);
            generateGraphvizCode(root->left, file);
        }
        if (root->right != NULL) 
        {
            fprintf(file, "\"%s\" -> \"%s\" [label=\"R\"];\n", root->key.data, root->right->key.data);
            generateGraphvizCode(root->right, file);
        }
    }
}

// Funkcia pre zapis grafickej tabulky symbolov do suboru
void createFile(local_symtab_t *symbolTable)
{
    FILE* dotFile = fopen("symtableWriteGraph.dot", "w");
    if (dotFile == NULL) 
    {
        perror("Subor nemozno otvorit\n");
        return;
    }

    // Začátek souboru s kódem Graphviz
    fprintf(dotFile, "digraph G {\n");

    // Generování kódu Graphviz
    generateGraphvizCode(symbolTable, dotFile);

    // Konec souboru s kódem Graphviz
    fprintf(dotFile, "}\n");

    // Zavření souboru
    fclose(dotFile);
}

void printSymbolTableRecursive(local_symtab_t *local_table) {
    if (local_table != NULL) {
        // Vypis uzlov stromu
        printSymbolTableRecursive(local_table->left);
        printf("Symbol: %s, Type: %d\n", local_table->key.data, local_table->type);
        printSymbolTableRecursive(local_table->right);
    }
}

void printSymbolTable(local_symtab_t *local_table) {
    printf("------ Symbol Table ------\n");
    printSymbolTableRecursive(local_table);
    printf("--------------------------\n");
}


int main() {
    token_t token;

    local_symtab_t* symbolTable;
    local_init(&symbolTable);


    printf(" TYPE\t\t| ATTRIBUTE\n");
    printf(" ↓↓ \t\t|    ↓↓↓   \n");

    while (1) 
    {
        token = getNextToken();

        if (token.type == TOK_EOF) 
        {
            printf("[ EOF ]\n");
            break;
        } 
        else if (token.type == TOK_IDENTIFIER) 
        {
            printf("[ Identifier: %s ]\n", token.attribute.str.data);
            local_insert(&symbolTable, &token.attribute.str, token.type);

            dstringFree(&(token.attribute.str));
        }
        else if (token.type == TOK_INT) 
        {
            printf("[ Integer: %d ]\n", token.attribute.number);
        } 
        else if (token.type == TOK_FLOAT) 
        {
            printf("[ Float: %f]\n", token.attribute.decimal);
        } 
        else if (token.type == TOK_EQUAL) 
        {
            printf("[ Equal ]\n");
        } 
        else if (token.type == TOK_NOT_EQUAL) 
        {
            printf("[ Not Equal ]\n");
        } 
        else if (token.type == TOK_LESSER) 
        {
            printf("[ Lesser ]\n");
        } 
        else if (token.type == TOK_LESSER_OR_EQUAL) 
        {
            printf("[ Lesser or Equal ]\n");
        } 
        else if (token.type == TOK_GREATER) 
        {
            printf("[ Greater ]\n");
        } 
        else if (token.type == TOK_PLUS) 
        {
            printf("[ Plus ]\n");
        } 
        else if (token.type == TOK_MINUS) 
        {
            printf("[ Minus ]\n");
        }
        
    }

    printSymbolTable(symbolTable);
    createFile(symbolTable);
    local_dispose(&symbolTable);
    
    return 0;
}