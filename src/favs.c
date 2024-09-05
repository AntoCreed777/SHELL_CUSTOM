#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include "favs.h"
#include "colores.h"
#include "comandos.h"

char *archivo_favs = "favs.csv";  //Archivo donde se guarda la lista de favoritos
const char *direccion_favs = "direccion_favs.txt";  //Archivo donde se guarda la direccion del archivo de favoritos
char ***cache_comandos = NULL; //Cache
int indice_inicio_anterior_linea_comando = 0;


void guardar_favs(){
    FILE *file = fopen(archivo_favs,"a+");
    if (file == NULL) {
        perror("Error al abrir el archivo");
        liberar_comandos();
        liberar_cache();
        exit(EXIT_FAILURE);
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
    printf(AMARILLO "Borrando Archivo: " AZUL "%s\n" RESET_COLOR, archivo_favs);

    if(access(archivo_favs, F_OK) != 0) return; //Si no existe el archivo

    if (remove(archivo_favs) != 0){
        perror("Error al eliminar el archivo de favoritos");
        exit(EXIT_FAILURE);
    }
    else{
        archivo_favs = "favs.csv";
        printf(AMARILLO "Archivo de favoritos eliminado exitosamente\n" RESET_COLOR);
        printf(AMARILLO "Se creará/usara un archivo de favoritos por defecto.\n" RESET_COLOR);

        if(access(direccion_favs, F_OK) != 0) return; //Si no existe el archivo

        if (remove(direccion_favs) != 0){
            perror("Error al eliminar el archivo de dirección de favoritos");
            exit(EXIT_FAILURE);
        }
    }
}

void mostrar_favs(){
    printf("Comandos Favoritos:\n");
    if(access(archivo_favs, F_OK) != 0) return; //Si no existe el archivo

    FILE *file = fopen(archivo_favs,"r");
    if (file == NULL) {
        perror("Error al abrir el archivo");
        liberar_comandos();
        liberar_cache();
        exit(EXIT_FAILURE);
    }

    int contador = 1;
    char buffer[BUFFER_SIZE];
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        buffer[strcspn(buffer, ";")] = 0;  // Eliminar el salto de línea
        printf(AZUL"%d: " ROJO "%s\n" RESET_COLOR, contador++, buffer);
    }

    fclose(file);
}

void cargar_favs(){
    FILE *file = fopen(archivo_favs,"r");
    if (file == NULL) {
        perror("Error al abrir el archivo");
        return;
    }

    int canticad_cache = 0;
    if(cache_comandos != NULL)
        for(int i=0;cache_comandos[i] != NULL;i++)
            canticad_cache++;

    indice_inicio_anterior_linea_comando = canticad_cache;

    char buffer[BUFFER_SIZE];
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        char *contexto_comando;

        char *token_comando = strtok_r(buffer,";\n",&contexto_comando); // Separo por ';' eliminando el '\n'

        while(token_comando != NULL){
            cache_comandos = (char***)realloc(cache_comandos,sizeof(char**) * (canticad_cache+1));

            if(cache_comandos == NULL){
                perror(ROJO "Error en la reasignación de memoria");
                exit(EXIT_FAILURE);
            }

            cache_comandos[canticad_cache] = NULL;

            int elemento = 0;
            char *contexto_elemento;
            char *token_elementos = strtok_r(token_comando," ",&contexto_elemento); // Separo por ' ' (espacio)

            while(token_elementos != NULL){

                cache_comandos[canticad_cache] = (char**)realloc(cache_comandos[canticad_cache],sizeof(char*) * (elemento+1));

                if(cache_comandos[canticad_cache] == NULL){
                    perror(ROJO "Error en la reasignación de memoria");
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
    
}

void eliminar_favs(int *numero, int numero_comandos_eliminar){
    for(int i = 0; i < numero_comandos_eliminar; i++){
        if(numero[i] <= 0) perror("Numero invalido.");
    }

    FILE *origenFile = fopen(archivo_favs, "r");
    FILE *origen_temporal = fopen("temp.csv", "w");

    if (origenFile == NULL || origen_temporal == NULL) {
        perror("No se pudo abrir el archivo de origen");
        liberar_comandos();
        liberar_cache();
        exit(EXIT_FAILURE);
    }

    int contador = 1;
    char buffer[BUFFER_SIZE];
    bool estado_eliminar = true;
    
    // Valido que el contador no sea igual a ninguno de los numeros.
    while(fgets(buffer, sizeof(buffer), origenFile)){
        estado_eliminar = true;
        for(int i = 0; i < numero_comandos_eliminar; i++)
            if(contador == numero[i]) estado_eliminar = false;
            
        if(estado_eliminar) fputs(buffer, origen_temporal);
        contador++;
    }

    fclose(origen_temporal);
    fclose(origenFile);

    remove(archivo_favs);
    rename("temp.csv", archivo_favs);
}

void buscar_favs(char *busqueda){
    FILE *origenFile = fopen(archivo_favs, "r");

    if (origenFile == NULL) {
        perror("No se pudo abrir el archivo de origen");
        liberar_comandos();
        liberar_cache();
        exit(EXIT_FAILURE);
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
        perror("Número invalido.");
        return;
    }

    FILE *origenFile = fopen(archivo_favs, "r");

    if (origenFile == NULL) {
        perror("No se pudo abrir el archivo de origen");
        liberar_comandos();
        liberar_cache();
        exit(EXIT_FAILURE);
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
                    perror(ROJO "Error en la reasignación de memoria");
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
            perror("Error al abrir el archivo de dirección de favoritos");
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
            perror("Error al asignar memoria para archivo_favs");
            liberar_comandos();
            liberar_cache();
            exit(EXIT_FAILURE);
        }
        fscanf(file, "%s", archivo_favs);
        fclose(file);
    }
}