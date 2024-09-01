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

bool is_number(char* string){
    for(int i=0;string[i] != '\0';i++)
        if(string[i] != '0' && string[i] != '1' && string[i] != '2' && string[i] != '3' && string[i] != '4' && string[i] != '5'
            && string[i] != '6' && string[i] != '7' && string[i] != '8' && string[i] != '9')
            return false;
    
    return true;
}