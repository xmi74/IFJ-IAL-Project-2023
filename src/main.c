#include <stdio.h>

#include "abstract_syntax_tree.h"


void generateAST(FILE *dotFile, ast_node_t *tree) 
{
    if (tree != NULL) 
    {
        //fprintf(dotFile, "  node%p [label=\"%s %d %f\"];\n", (void *)tree, tree->token.attribute.str.data, tree->token.attribute.intValue, tree->token.attribute.doubleValue);    
        if (tree->token.type == TOK_PLUS)
        {
            fprintf(dotFile, "  node%p [label=\"+\"];\n", (void *)tree);    
        }
        else if (tree->token.type == TOK_MINUS)
        {
            fprintf(dotFile, "  node%p [label=\"-\"];\n", (void *)tree);    
        }
        else if(tree->token.type == TOK_MUL)
        {
            fprintf(dotFile, "  node%p [label=\"*\"];\n", (void *)tree);
        }
        else if(tree->token.type == TOK_DIV)
        {
            fprintf(dotFile, "  node%p [label=\"/\"];\n", (void *)tree);
        }
        else if(tree->token.type == TOK_NOT_EQUAL)
        {
            fprintf(dotFile, "  node%p [label=\"!=\"];\n", (void *)tree);
        }
        else if(tree->token.type == TOK_LESSER)
        {
            fprintf(dotFile, "  node%p [label=\"<\"];\n", (void *)tree);
        }
        else if(tree->token.type == TOK_LESSER_OR_EQUAL)
        {
            fprintf(dotFile, "  node%p [label=\"<=\"];\n", (void *)tree);
        }
        else if(tree->token.type == TOK_GREATER)
        {
            fprintf(dotFile, "  node%p [label=\">\"];\n", (void *)tree);
        }
        else if(tree->token.type == TOK_GREATER_OR_EQUAL)
        {
            fprintf(dotFile, "  node%p [label=\">=\"];\n", (void *)tree);
        }
        else if(tree->token.type == TOK_EQUAL)
        {
            fprintf(dotFile, "  node%p [label=\"=\"];\n", (void *)tree);
        }
        else if(tree->token.type == TOK_ASSIGN)
        {
            fprintf(dotFile, "  node%p [label=\"==\"];\n", (void *)tree);
        }
        else if (tree->token.type == TOK_IDENTIFIER)
        {
            fprintf(dotFile, "  node%p [label=\"%s\"];\n", (void *)tree, tree->token.attribute.str.data);    
            //fprintf(dotFile, "  node%p [label=\"ID\"];\n", (void *)tree);    
        }
        else if (tree->token.type == TOK_INT)
        {
            fprintf(dotFile, "  node%p [label=\"%d\"];\n", (void *)tree, tree->token.attribute.intValue);  
            //fprintf(dotFile, "  node%p [label=\"INT\"];\n", (void *)tree);    
        }
        else if(tree->token.type == TOK_DOUBLE)
        {
            fprintf(dotFile, "  node%p [label=\"%f\"];\n", (void *)tree, tree->token.attribute.doubleValue);  
            //fprintf(dotFile, "  node%p [label=\"DOUBLE\"];\n", (void *)tree);    
        }
        else if(tree->token.type == TOK_STRING)
        {
            fprintf(dotFile, "  node%p [label=\"STRING\"];\n", (void *)tree);    
        }
        else if(tree->token.type == TOK_NOT)
        {
            fprintf(dotFile, "  node%p [label=\"!\"];\n", (void *)tree);    
        }
        
        if (tree->left != NULL) {
            fprintf(dotFile, "  node%p -> node%p [label=\"left\"];\n", (void *)tree, (void *)(tree->left));
            generateAST(dotFile, tree->left);
        }

        if (tree->right != NULL) {
            fprintf(dotFile, "  node%p -> node%p [label=\"right\"];\n", (void *)tree, (void *)(tree->right));
            generateAST(dotFile, tree->right);
        }
    }
}

void createFileAST(ast_node_t *tree) {
    FILE* dotFile = fopen("ast.dot", "w");
    if (dotFile == NULL) {
        perror("Subor ast.dot nemozno otvorit\n");
        return;
    }

    fprintf(dotFile, "digraph AST {\n");
    generateAST(dotFile, tree);
    fprintf(dotFile, "}\n");
    fclose(dotFile);
}

// Funkcia pre graficke znazornenie tabulky symbolov
void generateLocalSymtable(local_symtab_t* root, FILE* file) 
{
    if (root != NULL) 
    {
        if (root->left != NULL) 
        {
            fprintf(file, "\"%s\" -> \"%s\" [label=\"L\"];\n", root->key.data, root->left->key.data);
            generateLocalSymtable(root->left, file);
        }
        if (root->right != NULL) 
        {
            fprintf(file, "\"%s\" -> \"%s\" [label=\"R\"];\n", root->key.data, root->right->key.data);
            generateLocalSymtable(root->right, file);
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
    generateLocalSymtable(symbolTable, dotFile);

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

void scan(token_t *token)
{
    printf(" TYPE\t\t| ATTRIBUTE\n");
    printf(" ↓↓ \t\t|    ↓↓↓   \n");

    while (1)
    {
        *token = getNextToken();
        if (token->type == TOK_EOF) 
        {
            printf("[ EOF ]\n");
            break;
        } 
        else if (token->type == TOK_IDENTIFIER) 
        {
            printf("[ Identifier: %s ]\n", token->attribute.str.data);
            //dstringFree(&(token->attribute.str));
        }
        // KEYWORDS
        else if (token->type == TOK_KW_DOUBLE) 
        {
            printf("[ KW : Double , type includes nil ? : ");
            if (token->attribute.includesNil == true)
            {
                printf("YES ]\n");
            }
            else
            {
                printf("NO ]\n");
            }
        } 
        else if (token->type == TOK_KW_ELSE) 
        {
            printf("[ KW : else ]\n");
        } 
        else if (token->type == TOK_KW_FUNC) 
        {
            printf("[ KW : func ]\n");
        } 
        else if (token->type == TOK_KW_IF) 
        {
            printf("[ KW : if ]\n");
        } 
        else if (token->type == TOK_KW_INT) 
        {
            printf("[ KW : int , type includes nil ? : ");
            if (token->attribute.includesNil == true)
            {
                printf("YES ]\n");
            }
            else
            {
                printf("NO ]\n");
            }
        } 
        else if (token->type == TOK_KW_LET) 
        {
            printf("[ KW : let ]\n");
        }
        else if (token->type == TOK_KW_NIL)
        {
            printf("[ KW : nil ]\n");
        }
        else if (token->type == TOK_UNDERSCORE) 
        {
            printf("[ UNDERSCORE ]\n");
        } 
        else if (token->type == TOK_KW_RETURN) 
        {
            printf("[ KW : return ]\n");
        } 
        else if (token->type == TOK_KW_STRING) 
        {
            printf("[ KW : String , type includes nil? : ");
            if (token->attribute.includesNil == true)
            {
                printf("YES ]\n");
            }
            else
            {
                printf("NO ]\n");
            }
        } 
        else if (token->type == TOK_KW_VAR) 
        {
            printf("[ KW : var ]\n");
        } 
        else if (token->type == TOK_KW_WHILE) 
        {
            printf("[ KW : while ]\n");
        } 
        // KEYWORDS
        else if (token->type == TOK_INT) 
        {
            printf("[ Integer: %d ]\n", token->attribute.intValue);
        } 
        else if (token->type == TOK_DOUBLE) 
        {
            printf("[ Double: %f ]\n", token->attribute.doubleValue);
        } 
        else if (token->type == TOK_BLOCK_COM_END)
        {
            printf("[ BLOCK COMMENT END SIGN ]\n");
        }
        else if (token->type == TOK_DIV)
        {
            printf("[ DIVISION ]\n");
        }
        else if (token->type == TOK_MUL)
        {
            printf("[ MULTIPLY ]\n");
        }
        else if (token->type == TOK_BLOCK_COM_START)
        {
            printf("[ BLOCK COMMENT START SIGN ]\n");
        }
        else if (token->type == TOK_COMMENT)
        {
            printf("[ COMMENT ]\n");
        }
        else if (token->type == TOK_PLUS)
        {
            printf("[ PLUS ]\n");
        }
        else if (token->type == TOK_MINUS)
        {
            printf("[ MINUS ]\n");
        }
        else if (token->type == TOK_ARROW)
        {
            printf("[ ARROW ]\n");
        }
        else if (token->type == TOK_R_CRL_BRCKT)
        {
            printf("[ RIGHT CURLY BRACKET ]\n");
        }
        else if (token->type == TOK_L_CRL_BRCKT)
        {
            printf("[ LEFT CURLY BRACKET ]\n");
        }
        else if (token->type == TOK_R_BRCKT)
        {
            printf("[ RIGHT BRACKET ]\n");
        }
        else if (token->type == TOK_L_BRCKT)
        {
            printf("[ LEFT BRACKET ]\n");
        }
        else if (token->type == TOK_NOT)
        {
            printf("[ NOT ]\n");
        }
        else if (token->type == TOK_NOT_EQUAL)
        {
            printf("[ NOT EQUAL ]\n");
        }
        else if (token->type == TOK_LESSER)
        {
            printf("[ LESSER ]\n");
        }
        else if (token->type == TOK_LESSER_OR_EQUAL)
        {
            printf("[ LESSER OR EQUAL ]\n");
        }
        else if (token->type == TOK_GREATER)
        {
            printf("[ GREATER ]\n");
        }
        else if (token->type == TOK_GREATER_OR_EQUAL)
        {
            printf("[ GREATER OR EQUAL ]\n");
        }
        else if (token->type == TOK_EQUAL) 
        {
            printf("[ EQUAL ]\n");
        } 
        else if (token->type == TOK_ASSIGN)
        {
            printf("[ ASSIGN ]\n");
        }
        else if (token->type == TOK_COLON)
        {
            printf("[ COLON ]\n");
        }
        else if (token->type == TOK_COMMA) 
        {
            printf("[ COMMA ]\n");
        }
        else if (token->type == TOK_QUEST_MARK)
        {
            printf("[ QUESTION MARK (after type declaration) ]\n");
        }
        else if (token->type == TOK_DOUBLE_QUEST_MARK)
        {
            printf("[ DOUBLE QUESTION MARK ]\n");
        }
        else if (token->type == TOK_STRING)
        {
            printf("[ String : %s ]\n", token->attribute.str.data);
        }
        else if (token->type == TOK_UNDERSCORE)
        {
            printf("[ UNDERSCORE ]\n");
        }
        else if (token->type == TOK_EOL)
        {
            printf("[ EOL ]\n");
        }
        freeToken(token);
    }
}


int main() {
    token_t token;

    /*local_symtab_t* symbolTable;
    local_init(&symbolTable); */

    scan(&token);    
    
    /*while (1)
    {
        token = getNextToken();
        if (token.type == TOK_IDENTIFIER)
        {
            symbolTable = local_insert(symbolTable, &token.attribute.str, token.type);
        }
        else if (token.type == TOK_EOF)
        {
            break;
        }
        freeToken(&token);
    }
    
    int symtabHeight = heightL(symbolTable);
    int symtabBalance = balanceL(symbolTable);

    printf("Vyska tabulky symbolov: %d\n", symtabHeight);
    printf("Vyvazenost : %d\n\t{-1, 0, 1} -> vyvazena\n", symtabBalance);*/

    /*printSymbolTable(symbolTable);
    createFile(symbolTable);
    local_dispose(&symbolTable);*/

    //ast_node_t *myTree = NULL;
    /*ast_items_t items;
    //ast_node_t *rootNode;
    
    items_init(&items);*/

    /*ast_node_t *leftLeaf;
    ast_node_t *rightLeaf;
    ast_node_t *additionNode;
    leftLeaf = make_leaf("2");
    rightLeaf = make_leaf("3");
    additionNode = make_tree("+", leftLeaf, rightLeaf);*/

    /*token_t token;
    token_t previous_token;
    initToken(&previous_token);
    ast_node_t *rootNode = NULL;
    ast_node_t *currentNode = NULL;
    ast_init(rootNode);


    // S -> id = e
    // Ei -> Ej + Ek
    // Ei -> Ej * Ek
    // Ei -> id
    // INPUT : 5 + B + C + D - X
    token = getNextToken();
    if (token.type == TOK_IDENTIFIER || token.type == TOK_INT || token.type == TOK_DOUBLE || token.type == TOK_STRING) 
    {
        ast_node_t *leftNode = make_leaf(token);
        currentNode = leftNode;
    }
    while (1) 
    {
        token = getNextToken();

        if (token.type == TOK_PLUS || token.type == TOK_MUL || token.type == TOK_DIV ||
            token.type == TOK_LESSER_OR_EQUAL || token.type == TOK_MINUS) 
        {
            previous_token = token;
            token = getNextToken();

            if (token.type == TOK_IDENTIFIER) {
                ast_node_t *rightNode = make_leaf(token);
                ast_node_t *additionNode = make_tree(previous_token, currentNode, rightNode);
                currentNode = additionNode;
            }
        }
        else if (token.type == TOK_EOF) 
        {
            break;
        }
    }

        // Kořenový uzel bude poslední vytvořený uzel
    rootNode = currentNode;
    
    freeToken(&token);

    createFileAST(rootNode);

    // Prejdenie AST v postorder a pridanie uzlov do items
    ast_postorder(rootNode, &items);*/

    //ast_dispose(rootNode);

    return 0;
}
