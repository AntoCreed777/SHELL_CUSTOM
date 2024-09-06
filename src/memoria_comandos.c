#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include "memoria_comandos.h"

char ***comandos = NULL;
char ***cache_comandos = NULL;
pid_t c_pid = -1;
int indice_inicio_anterior_linea_comando = 0;

char *archivo_favs = "favs.csv";  //Archivo donde se guarda la lista de favoritos
const char *direccion_favs = "direccion_favs.txt";  //Archivo donde se guarda la direccion del archivo de favoritos




void liberar_comandos(){
    if(comandos == NULL) return;
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

