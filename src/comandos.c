#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdbool.h>
#include "comandos.h"
#include "interfaz.h"
#include "constantes.h"
#include "utils.h"
#include "memoria_comandos.h"
#include "favs.h"
#include "internos_comandos.h"
#include "externos_comandos.h"


char ***entrada_comandos(){
    mostrar_prompt();

    //Entrada de linea de comando
    size_t numero_bytes = 0;    //Tamano del buffer
    char *cadena = NULL;
    int buffer_leido = getline(&cadena, &numero_bytes, stdin);

    
    if (buffer_leido == -1){
        printf(ROJO "ERROR AL LEER INPUT" RESET_COLOR);
        liberar_memoria_programa();
        return NULL;
    }
    if(is_empty_linea(cadena)) {    //Si no se ingresa ningun comando, (solamente ingresa '\n')
        free(cadena);
        return NULL;
    }

    eliminar_tabs(cadena); // Eliminar los tabs de la cadena

    //Extraccion de las distintas partes del comando
    char ***comandos = NULL;
    int comando = 0;
    char *contexto_comando;

    char *token_comando = strtok_r(cadena,";\n",&contexto_comando); // Separo por ';' eliminando el '\n'

    while(token_comando != NULL){

        comandos = (char***)realloc(comandos,sizeof(char**) * (comando+1));

        if(comandos == NULL){
            perror(ROJO "Error en la reasignación de memoria" RESET_COLOR);
            liberar_memoria_programa();
            exit(EXIT_FAILURE);
        }
        comandos[comando] = NULL;

        int elemento = 0;
        char *contexto_elemento;
        char *token_elementos = strtok_r(token_comando," ",&contexto_elemento); // Separo por ' ' (espacio)

        while(token_elementos != NULL){

            comandos[comando] = (char**)realloc(comandos[comando],sizeof(char*) * (elemento+1));

            if(comandos[comando] == NULL){
                perror(ROJO "Error en la reasignación de memoria" RESET_COLOR);
                liberar_memoria_programa();
                exit(EXIT_FAILURE);
            }

            comandos[comando][elemento] = strdup(token_elementos);

            elemento++;
            token_elementos = strtok_r(NULL," ",&contexto_elemento);
        }

        // Añadimos un NULL al final del array para marcar el fin del comando
        comandos[comando] = (char **)realloc(comandos[comando], sizeof(char*) * (elemento+1));
        comandos[comando][elemento] = NULL;

        comando++;
        token_comando = strtok_r(NULL,";\n",&contexto_comando);
    }

    // Añadimos un NULL al final del array para marcar el fin de los comandos
    comandos = (char ***)realloc(comandos, sizeof(char**) * (comando + 1));
    comandos[comando] = NULL;

    comandos_validos = (bool *)malloc(sizeof(bool) * comando);
    if (comandos_validos == NULL) {
        printf(ROJO "Error en la asignación de memoria" RESET_COLOR);
        liberar_memoria_programa();
        exit(EXIT_FAILURE);
    }

    free(cadena); // Liberar la cadena original

    return comandos;
}



void guardar_comandos_cache(){
    int num_comandos = 0, num_invalidos = 0, num_cache = 0;

    if (cache_comandos != NULL)
        for(int i=0;cache_comandos[i] != NULL;i++) 
            num_cache++;

    // Procesar los nuevos comandos para guardarlos
    while(comandos[num_comandos + num_invalidos] != NULL) {
        bool guardado_ignorado = 
            //comandos_validos[num_comandos + num_invalidos] == false ||        // No se porque no funciona aqui pero si despues
            is_in_cache(comandos[num_comandos + num_invalidos], cache_comandos) ||
            strcmp(comandos[num_comandos + num_invalidos][0],"!!") == 0 ||
            strcmp(comandos[num_comandos + num_invalidos][0],"favs") == 0;

        if(guardado_ignorado) {
            num_invalidos++;
            continue;
        }

        if (comandos_validos[num_comandos + num_invalidos] == false) {
            num_invalidos++;
            continue;
        }

        int posicion_cache = num_cache + num_comandos;

        // Reasignar memoria para el nuevo comando
        cache_comandos = (char***)realloc(cache_comandos,sizeof(char**) * (posicion_cache + 1));
        if(cache_comandos == NULL){
            perror(ROJO "Error en la reasignación de memoria" RESET_COLOR);
            liberar_memoria_programa();
            exit(EXIT_FAILURE);
        }

        cache_comandos[posicion_cache] = NULL;

        int num_elementos = 0;
        
        // Copiar los elementos del comando
        while(comandos[num_comandos + num_invalidos][num_elementos] != NULL){    // Elementos del Comando
            cache_comandos[posicion_cache] = (char**)realloc(cache_comandos[posicion_cache],sizeof(char*) * (num_elementos+1));
            
            if(cache_comandos[posicion_cache] == NULL){
                perror(ROJO "Error en la reasignación de memoria" RESET_COLOR);
                liberar_memoria_programa();
                exit(EXIT_FAILURE);
            }

            cache_comandos[posicion_cache][num_elementos] = strdup(comandos[num_comandos + num_invalidos][num_elementos]);
            num_elementos++;
        }
        // Terminar el array de elementos del comando con NULL
        cache_comandos[posicion_cache] = (char**)realloc(cache_comandos[posicion_cache],sizeof(char*) * (num_elementos+1));
        cache_comandos[posicion_cache][num_elementos] = NULL;
        num_comandos++;
    }

    // Terminar el array de comandos anteriores con NULL
    cache_comandos = (char***)realloc(cache_comandos,sizeof(char**) * (num_comandos + num_cache +1));
    cache_comandos[num_comandos + num_cache] = NULL;
}


void guardar_comandos_anteriores(){
    if(strcmp(comandos[0][0],"!!") == 0 || strcmp(comandos[0][0],"favs") == 0) return;
    
    if(comandos_anteriores != NULL) liberar_comandos_anteriores();
    comandos_anteriores = NULL;

    int num_comandos = 0, num_invalidos = 0;

    while(comandos[num_comandos + num_invalidos] != NULL) {   // Comandos
        if(strcmp(comandos[num_comandos + num_invalidos][0],"!!") == 0 || strcmp(comandos[num_comandos + num_invalidos][0],"favs") == 0) {
            num_invalidos++;
            continue;
        }

        comandos_anteriores = (char***)realloc(comandos_anteriores,sizeof(char**) * (num_comandos+1));
        
        if(comandos_anteriores == NULL){
            perror(ROJO "Error en la reasignación de memoria");
            liberar_memoria_programa();
            exit(EXIT_FAILURE);
        }

        comandos_anteriores[num_comandos]= NULL;

        int num_elementos = 0;

        while(comandos[num_comandos + num_invalidos][num_elementos] != NULL){    // Elementos del Comando
            comandos_anteriores[num_comandos] = (char**)realloc(comandos_anteriores[num_comandos],sizeof(char*) * (num_elementos+1));
            
            if(comandos_anteriores[num_comandos] == NULL){
                perror(ROJO "Error en la reasignación de memoria");
                exit(EXIT_FAILURE);
            }

            comandos_anteriores[num_comandos][num_elementos] = strdup(comandos[num_comandos + num_invalidos][num_elementos]);
            num_elementos++;
        }
        comandos_anteriores[num_comandos] = (char**)realloc(comandos_anteriores[num_comandos],sizeof(char*) * (num_elementos+1));
        comandos_anteriores[num_comandos][num_elementos] = NULL;
        num_comandos++;
    }
    comandos_anteriores = (char***)realloc(comandos_anteriores,sizeof(char**) * (num_comandos+1));
    comandos_anteriores[num_comandos] = NULL;
}
