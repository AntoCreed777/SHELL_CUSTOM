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
    if (cache_comandos == NULL || !cache_comandos[0]){
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

void eliminar_favs(int *numero_comando_eliminar, int cantidad_numeros_eliminar){

    int tam_cache = 0;

    if (cache_comandos != NULL)
        for(int i=0;cache_comandos[i] != NULL;i++) 
            tam_cache++;
    
    int tam_nuevo = tam_cache - cantidad_numeros_eliminar;

    for(int i = 0; i < cantidad_numeros_eliminar; i++){
        if(numero_comando_eliminar[i] <= 0 || numero_comando_eliminar[i] > tam_cache){
            printf(ROJO "Numero invalido\n" RESET_COLOR);
            return;
        }
    }

    char ***aux = (char***)malloc(tam_nuevo  * sizeof(char**));

    // Validacion si el numero es el correcto para eliminarlo
    bool estado_eliminar = false;

    for(int contador = 0, aux_index = 0; contador < tam_cache; contador++) {
        estado_eliminar = false;
        for(int j = 0; j < cantidad_numeros_eliminar; j++) {
            if(contador + 1 == numero_comando_eliminar[j]) {
                estado_eliminar = true;
                break;
            }
        }
        if(!estado_eliminar) {
            aux[aux_index] = NULL;  // Inicializamos el aux[aux_index]
            for(int i = 0; cache_comandos[contador][i] != NULL; i++) {
                aux[aux_index] = (char **)realloc(aux[aux_index], (i + 1) * sizeof(char*));
                if(aux[aux_index] == NULL) {
                    printf("Error al reservar memoria");
                    liberar_comandos();
                    liberar_cache();
                    liberar_comandos_anteriores();
                    exit(EXIT_FAILURE);
                }
                aux[aux_index][i] = strdup(cache_comandos[contador][i]);
            }
            aux_index++;
        }
    }

    char ***aux2;
    aux2 = cache_comandos;
    cache_comandos = aux;

    for(int i = 0; aux2[i] != NULL; i++){
        for(int j = 0; aux2[i][j] != NULL; j++){
            free(aux2[i][j]);
        }
        free(aux2[i]);
    }
    free(aux2);
}

void buscar_favs(char *busqueda){
    for(int i=0;cache_comandos[i] != NULL;i++){
        for(int j=0;cache_comandos[i][j] != NULL;j++){
            if(strstr(cache_comandos[i][j], busqueda) != NULL){
                printf(AZUL"%d: " ROJO, i+1);
                for(int k=0;cache_comandos[i][k] != NULL;k++){
                    printf("%s ",cache_comandos[i][k]);
                }
                printf("\n" RESET_COLOR);
                break;
            }
        }
    }
}

void ejecutar_favs(int numero){
    int tam_cache = 0;

    if (cache_comandos != NULL)
        for(int i=0;cache_comandos[i] != NULL;i++) 
            tam_cache++;

    if(numero <= 0 || numero > tam_cache){
        printf(ROJO "Numero invalido\n" RESET_COLOR);
        return;
    }

    if(manejar_comandos_internos(cache_comandos[numero-1])) return;
    manejar_comandos_externos(cache_comandos[numero-1]);
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
