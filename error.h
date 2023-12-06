/**
 * IFJ-IAL Projekt 2023
 *
 * @file error.h
 * @brief Definicie errorov
 *
 * @author Igor Mikula (xmikul74)
 * @author Marko Olesak (xolesa00)
 * @author Jan Findra (xfindr01)
 * @author Tomas Arlt (xarltt00)
 */

#include <stdio.h>
#include <stdlib.h>

#ifndef _ERROR_H
#define _ERROR_H

// Errorove navratove hodnoty

#define OK 0                      // preklad prebehol bez chyb
#define SCANNER_ERR 1             // chyba v ramci lexiklanej analyzy
#define SYNTAX_ERR 2              // chyba v ramci syntaktickej analyzy
#define FUNCTION_DEFINITION_ERR 3 // nedefinovana funkcia, redefinicia premennej
#define FUNCTION_USAGE_ERR 4      // nespravny pocet/typ parametrov funkcie alebo nespravny typ navratovej hodnoty
#define VARIABLE_DEFINITION_ERR 5 // nedefinovana alebo neinicializovana premenna
#define FUNCTION_RETURN_ERROR 6   // chybajuci/prebyvajuci vyraz pri navrate z funkcie
#define TYPE_COMPATIBILITY_ERR 7  // chyba typovej kompatibility pre vyrazoch
#define TYPE_DEDUCTION_ERR 8      // nie je uvedeny typ a neda sa odvodit
#define OTHER_ERR 9               // ostatne semanticke chyby
#define INTERN_ERR 99             // interna chyba prekladaca

#endif

/**
 * @brief vratenie chybovej navratovej hodnoty a ukoncenie programu
 * 
 * @param GLOBAL_ERROR_VALUE chybova hodnota
*/
void returnError(int GLOBAL_ERROR_VALUE);
