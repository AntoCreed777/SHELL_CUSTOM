#include "utils.h"
#include <stdlib.h>
#include <stdbool.h>

bool is_empty_linea(char* linea){
    if (!linea)
        return true;
    int contador = 0;
    //Se agrega la verificacion de \0 para asegurarnos que se pueda reutilizar esta linea en diferentes casos.
    while(linea[contador] != '\n' && linea[contador] != '\0'){  
        if (linea[contador] != ' ')
            return false;
        contador++;
    }
    return true;
}

char *eliminar_tabs(char *linea){
    for(int contador = 0; linea[contador] != '\0';contador++)
        if(linea[contador] == '\t')
            linea[contador] = ' ';
    
    return linea;
}