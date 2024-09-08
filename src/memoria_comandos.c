#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include "memoria_comandos.h"

char ***comandos = NULL;

// 'cache_comandos' guarda todos los comandos que se ingresan sin repretir en la instancia actual de la shell,
// mientras que 'comandos_anteriores' guarda los comandos que se ingresan en la ultima linea de comando
char ***cache_comandos = NULL;
char ***comandos_anteriores = NULL;

pid_t c_pid = -1;
int indice_inicio_anterior_linea_comando = 0;

char *archivo_favs = NULL;  //Archivo donde se guarda la lista de favoritos
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

void liberar_comandos_anteriores(){
    if(comandos_anteriores == NULL) return;
    for(int i=0;comandos_anteriores[i] != NULL;i++) {
        for(int j=0;comandos_anteriores[i][j] != NULL;j++) free(comandos_anteriores[i][j]);
        free(comandos_anteriores[i]);
    }
    free(comandos_anteriores);
}

void liberar_memoria_programa(){
    liberar_comandos();
    liberar_cache();
    liberar_comandos_anteriores();
    free(archivo_favs);
}