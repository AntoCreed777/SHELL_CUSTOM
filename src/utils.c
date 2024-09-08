#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "utils.h"

bool is_empty_linea(char* linea){
    if (linea == NULL) return true;
    int contador = 0;
    //Se agrega la verificacion de \0 para asegurarnos que se pueda reutilizar esta linea en diferentes casos.
    while(linea[contador] != '\n' && linea[contador] != '\0'){  
        if (linea[contador] != ' ' && linea[contador] != '\t')
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

bool is_in_cache(char** comandos, char*** cache_comand){
    if (cache_comand == NULL)
        return false;
    int i = 0;
    while(cache_comand[i]){
        int j = 0;
        while (cache_comand[i][j] && comandos[j]){

            if (strcmp(cache_comand[i][j], comandos[j]) != 0)
                break;

            j++;
            
        }
        if (cache_comand[i][j] == NULL && comandos[j] == NULL){
            return true;
        }
        i++;
    }
    return false;
}
