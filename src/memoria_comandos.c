#include <stdio.h>
#include <stdlib.h>
#include "memoria_comandos.h"

char ***comandos = NULL;
char ***cache_comandos = NULL;



void liberar_comandos(){
    for (int i = 0; comandos[i] != NULL; i++) {
        for (int j = 0; comandos[i][j] != NULL; j++) free(comandos[i][j]);
        free(comandos[i]);
    }
    free(comandos);
}

void liberar_cache(){
    if(cache_comandos == NULL) return;
    for(int i=0;cache_comandos[i] != NULL;i++) {
        for(int j=0;cache_comandos[i][j] != NULL;j++) free(cache_comandos[i][j]);
        free(cache_comandos[i]);
    }
    free(cache_comandos);
}

