/**
 * IFJ-IAL Projekt 2023
 * 
 * @file dynamic_string.c
 * @brief Implementacia struktury a funkcii pre manipulaciu s dynamickym stringom
 * 
 * @author Igor Mikula (xmikul74)
 * @author Marko Olesak (xolesa00)
 * @author Jan Findra (xfindr01)
 * @author Tomas Arlt (xarltt00)
*/

#include "error.h"

void returnError(int GLOBAL_ERROR_VALUE)
{
    switch(GLOBAL_ERROR_VALUE)
    {
        case SCANNER_ERR:
            fprintf(stderr, "ERROR: Lexikalna Analyza\n");
            exit(SCANNER_ERR);
            break;
        case SYNTAX_ERR:           
            fprintf(stderr, "ERROR: Syntakticka Analyza\n");
            exit(SYNTAX_ERR);
            break;
        case FUNCTION_DEFINITION_ERR:
            fprintf(stderr, "ERROR: Definicia funkcie\n");
            exit(FUNCTION_DEFINITION_ERR);
            break;
        case FUNCTION_USAGE_ERR:
            fprintf(stderr, "ERROR: Pouzitie funkcie\n");
            exit(FUNCTION_USAGE_ERR);
            break;
        case VARIABLE_DEFINITION_ERR:
            fprintf(stderr, "ERROR: Definicia premennej\n");
            exit(VARIABLE_DEFINITION_ERR);
            break;
        case FUNCTION_RETURN_ERROR:
            fprintf(stderr, "ERROR: Navratova hodnota funkcie\n");
            exit(FUNCTION_RETURN_ERROR);
            break;
        case COMPATIBILITY_ERR:
            fprintf(stderr, "ERROR: Kompatibilita\n");
            exit(COMPATIBILITY_ERR);
            break;
        case TYPE_DEDUCTION_ERR:
            fprintf(stderr, "ERROR: Dedukcia typov\n");
            exit(TYPE_DEDUCTION_ERR);
            break;
        case OTHER_ERR:
            fprintf(stderr, "ERROR: Iny error\n");
            exit(OTHER_ERR);
            break;
        case INTERN_ERR:   
            fprintf(stderr, "ERROR: Interny error\n");
            exit(INTERN_ERR);
            break;
        default:
            break;
    }
}
