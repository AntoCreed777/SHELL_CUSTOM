#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "utils.h"
#include "memoria_comandos.h"

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

bool is_in_cache(char** comandos){
    if (cache_comandos == NULL || cache_comandos[0] == NULL) return false;

    for(int num_comando = 0; cache_comandos[num_comando] != NULL; num_comando++){   // Comandos en cache
        for(int num_elemento = 0; cache_comandos[num_comando][num_elemento] != NULL; num_elemento++){   // Elementos del Comando
            if(comandos[num_elemento] == NULL) return false;    // Si el comando no tiene la misma cantidad de elementos
            if(strcmp(cache_comandos[num_comando][num_elemento],comandos[num_elemento]) != 0)   // Si los elementos no son iguales
                break;
            if(cache_comandos[num_comando][num_elemento+1   ] == NULL && comandos[num_elemento+1] == NULL) // Si llegamos al final de ambos comandos
                return true;
        }
    }
    return false;
}
