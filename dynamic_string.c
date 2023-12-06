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

#include "dynamic_string.h"

#define INITIAL_SIZE 16


// Inicializacia dynamickeho retazca
void dstringInit(string_t *str) {
    str->data = (char *)malloc(INITIAL_SIZE * sizeof(char));
    if (str->data == NULL) {
        fprintf(stderr, "ERROR : Alokacia dynamickeho stringu (dstringInit()).\n");
        exit(EXIT_FAILURE);
    }
    str->data[0] = '\0';
    str->length = 0;
    str->capacity = INITIAL_SIZE;
}

// Uvolnenie pamati dynamickeho retazca
void dstringFree(string_t *str) {
    if (str != NULL || str->data != NULL)
    {
        free(str->data);
        str->data = NULL;
        str->length = 0;
        str->capacity = 0;
    }
}

// Pridanie znaku na koniec retazca
void dstringAppend(string_t *str, char c) {
    if (str->length + 1 >= str->capacity) {
        size_t newCapacity = (str->capacity == 0) ? 1 : (str->capacity * 2);
        char *newData = (char *)realloc(str->data, newCapacity);
        if (newData == NULL) {
            fprintf(stderr, "ERROR : Alokacia dynamickeho stringu (dstringAppend()).\n");
            exit(EXIT_FAILURE);
        }
        str->data = newData;
        str->capacity = newCapacity;
    }

    // Přidáme znak na konec řetězce a aktualizujeme délku
    str->data[str->length] = c;
    str->length++;
    str->data[str->length] = '\0'; // Ujistíme se, že řetězec je ukončen nulovým znakem
}

// Prevedenie celeho retazca zo source -> destination 
// (funguje ako strcpy pre dynamicky retazec)
int dstringCopy(string_t *destination, string_t *source)
{
    if (source == NULL || destination == NULL || source->data == NULL || destination->data == NULL)
    {
        return 99;
    }
    if (source->length >= destination->capacity)
    {
        destination->data = realloc(destination->data, source->length + 1);
        if (!destination->data)
        {
            return 99;
        }
        destination->capacity = source->length + 1;
    }
    strcpy(destination->data, source->data);
    destination->length = source->length;
    return 1;
}

// Porovnanie retazcov str_1 a str_2
// (funguje ako strcmp pre dynamicky retazec)
int dstringCompare(string_t *str_1, string_t *str_2)
{
    return strcmp(str_1->data, str_2->data);
}
