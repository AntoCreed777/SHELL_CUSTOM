#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include "favs.h"
#include "constantes.h"
#include "memoria_comandos.h"
#include "comandos.h"


void guardar_favs(){
    FILE *file = fopen(archivo_favs,"w");
    if (file == NULL) {
        perror(ROJO "Error al abrir el archivo" RESET_COLOR);
        return;
    }

    
    for(int i=0; cache_comandos[i] != NULL;i++){
        for(int j=0;cache_comandos[i][j] != NULL;j++){
            fprintf(file,"%s ",cache_comandos[i][j]);
        }
        fprintf(file,";\n");
    }

    fclose(file);
}

void borrar_favs(){
    printf(AMARILLO "Borrando memoria de los favs\n");
    liberar_cache();
    cache_comandos = NULL;

}

void mostrar_favs(){
    if (cache_comandos == NULL){
        printf(AMARILLO "No hay comandos favoritos\n" RESET_COLOR);
        return;
    }

    int i = 0;
    printf("Comandos Favoritos:\n");
    while(cache_comandos[i]){
        int j = 0;
        printf(AZUL "%d:  ", i + 1);
        while (cache_comandos[i][j]){
            printf(ROJO "%s ", cache_comandos[i][j]);
            j++; 
        }
        printf("\n");
        i++;
    }
    return;
}

bool cargar_favs(){
    FILE *file = fopen(archivo_favs,"r");
    if (file == NULL) {
        perror(ROJO "Error al abrir el archivo" RESET_COLOR);
        return false;
    }

    int canticad_cache = 0;
    if(cache_comandos != NULL)
        for(int i=0;cache_comandos[i] != NULL;i++)
            canticad_cache++;

    char buffer[BUFFER_SIZE];
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        char *contexto_comando;

        char *token_comando = strtok_r(buffer,";\n",&contexto_comando); // Separo por ';' eliminando el '\n'

        while(token_comando != NULL){
            cache_comandos = (char***)realloc(cache_comandos,sizeof(char**) * (canticad_cache+1));

            if(cache_comandos == NULL){
                perror(ROJO "Error en la reasignación de memoria" RESET_COLOR);
                liberar_comandos();
                exit(EXIT_FAILURE);
            }

            cache_comandos[canticad_cache] = NULL;

            int elemento = 0;
            char *contexto_elemento;
            char *token_elementos = strtok_r(token_comando," ",&contexto_elemento); // Separo por ' ' (espacio)

            while(token_elementos != NULL){

                cache_comandos[canticad_cache] = (char**)realloc(cache_comandos[canticad_cache],sizeof(char*) * (elemento+1));

                if(cache_comandos[canticad_cache] == NULL){
                    perror(ROJO "Error en la reasignación de memoria" RESET_COLOR);
                    liberar_comandos();
                    liberar_cache();
                    exit(EXIT_FAILURE);
                }

                cache_comandos[canticad_cache][elemento] = strdup(token_elementos);

                elemento++;
                token_elementos = strtok_r(NULL," ",&contexto_elemento);
            }

            // Añadimos un NULL al final del array para marcar el fin del comando
            cache_comandos[canticad_cache] = (char **)realloc(cache_comandos[canticad_cache], sizeof(char*) * (elemento+1));
            cache_comandos[canticad_cache][elemento] = NULL;

            token_comando = strtok_r(NULL,";\n",&contexto_comando);

            canticad_cache++;

        }

        // Añadimos un NULL al final del array para marcar el fin de los comandos
        cache_comandos = (char ***)realloc(cache_comandos, sizeof(char**) * (canticad_cache + 1));
        cache_comandos[canticad_cache] = NULL;
    }

    return true;
    
}

void eliminar_favs(int *numero_comando_eliminar, int cantidad_comandos_eliminar){
    for(int i = 0; i < cantidad_comandos_eliminar; i++){
        if(numero_comando_eliminar[i] <= 0) perror(ROJO "Numero invalido." RESET_COLOR);
    }

    FILE *origenFile = fopen(archivo_favs, "r");
    FILE *origen_temporal = fopen("temp.csv", "w");

    if (origenFile == NULL || origen_temporal == NULL) {
        perror(ROJO "No se pudo abrir el archivo de origen" RESET_COLOR);
        return;
    }

    int contador = 1;
    char buffer[BUFFER_SIZE];
    bool estado_eliminar = false;
    
    // Valido que el contador no sea igual a ninguno de los numeros.
    while(fgets(buffer, sizeof(buffer), origenFile)){
        estado_eliminar = false;
        for(int i = 0; i < cantidad_comandos_eliminar; i++)
            if(contador == numero_comando_eliminar[i]) {
                estado_eliminar = true;
                numero_comando_eliminar[i] = -1;
            }
            
        if(!estado_eliminar) fputs(buffer, origen_temporal);
        contador++;
    }

    for(int i = 0; i < cantidad_comandos_eliminar; i++)
        if(numero_comando_eliminar[i] > 0)
            printf(AMARILLO "Numero invalido: %d\n" RESET_COLOR, numero_comando_eliminar[i]);

    fclose(origen_temporal);
    fclose(origenFile);

    remove(archivo_favs);
    rename("temp.csv", archivo_favs);
}

void buscar_favs(char *busqueda){
    FILE *origenFile = fopen(archivo_favs, "r");

    if (origenFile == NULL) {
        perror(ROJO "No se pudo abrir el archivo de origen" RESET_COLOR);
        return;
    }


    char buffer[BUFFER_SIZE];
    int contador = 1;
    while(fgets(buffer, sizeof(buffer), origenFile) != NULL){
        if(strstr(buffer, busqueda) != NULL){
            buffer[strcspn(buffer, ";")] = 0;
            printf(AZUL"%d: " ROJO "%s\n" RESET_COLOR, contador, buffer);
        }
        contador++;
    }
}

void ejecutar_favs(int numero){
    // En caso de que atoi arroje un numero menor a 0
    if(numero <= 0){
        perror(ROJO "Número invalido." RESET_COLOR);
        return;
    }

    FILE *origenFile = fopen(archivo_favs, "r");

    if (origenFile == NULL) {
        perror(ROJO "No se pudo abrir el archivo de origen" RESET_COLOR);
        return;
    }

    char buffer[BUFFER_SIZE];
    int contador = 1;
    // Buscamos la posicion de el comando
    while(fgets(buffer, sizeof(buffer), origenFile) != NULL){
        if(contador++ == numero){
            // Quitamos valores sobrantes e innecesarios
            buffer[strcspn(buffer, ";")] = 0;
            buffer[strcspn(buffer, "\n")] = '\0';

            char **comando_ejecutar = NULL;
            int tamaño = 0;

            char *token = strtok(buffer, " ");
            // Creamos el array de comandos para poder pasarlo a manejo de comandos
            while (token != NULL) {
                comando_ejecutar = (char **)realloc(comando_ejecutar, sizeof(char *) * ++tamaño);
                if (comando_ejecutar == NULL) {
                    perror(ROJO "Error en la reasignación de memoria" RESET_COLOR);
                    liberar_comandos();
                    liberar_cache();
                    exit(EXIT_FAILURE);
                }
                comando_ejecutar[tamaño - 1] = strdup(token);
                token = strtok(NULL, " ");
            }
            // Ultimo comando lo dejamos en NULl
            comando_ejecutar = (char **)realloc(comando_ejecutar, sizeof(char *) * (tamaño + 1));
            comando_ejecutar[tamaño] = NULL;

            if(manejar_comandos_internos(comando_ejecutar)) continue;
            manejar_comandos_externos(comando_ejecutar);

            // Liberar memoria xd
            for (int i = 0; i < tamaño; i++) {
                free(comando_ejecutar[i]);
            }
            free(comando_ejecutar);
        }
    }
    fclose(origenFile);
}

void guardar_ruta_favs(){
        FILE *file = fopen(direccion_favs, "w");
        if (file == NULL) {
            perror(ROJO "Error al abrir el archivo de dirección de favoritos" RESET_COLOR);
            liberar_comandos();
            liberar_cache();
            exit(EXIT_FAILURE);
        }
        fputs(archivo_favs, file);
        fclose(file);
}

void cargar_ruta_favs(){
    printf(AMARILLO "Cargando archivo de favoritos...\n" RESET_COLOR);
    FILE *file = fopen(direccion_favs, "r");
    if (file == NULL) {
        perror(ROJO "Error al abrir el archivo de dirección de favoritos" RESET_COLOR);
        printf(AMARILLO "Se creará/usara un archivo de favoritos por defecto.\n" RESET_COLOR);
    }
    else{
        archivo_favs = (char*)malloc(256 * sizeof(char));  // Reserva espacio para 256 caracteres
        if (archivo_favs == NULL) {
            perror(ROJO "Error al asignar memoria para archivo_favs" RESET_COLOR);
            liberar_comandos();
            liberar_cache();
            exit(EXIT_FAILURE);
        }
        fscanf(file, "%s", archivo_favs);
        fclose(file);
    }
}
