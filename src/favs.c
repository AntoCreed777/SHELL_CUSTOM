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

    if(cache_comandos == NULL || cache_comandos[0] == NULL){
        printf(AMARILLO "No hay comandos favoritos para guardar\n" RESET_COLOR);
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

    // Contamos los comandos en cache
    int canticad_cache = 0;
    if(cache_comandos != NULL)
        for(int i=0;cache_comandos[i] != NULL;i++)
            canticad_cache++;


    char buffer[BUFFER_SIZE];

    // Empezamos a leer los comandos de los archivos
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        // Creamos comparador
        int largo_comando_token[canticad_cache];
        int comparador_token[canticad_cache];

        for(int i = 0; i < canticad_cache; i++){
            largo_comando_token[i] = 0;
            comparador_token[i] = 0;
        }
        
        // Creamos el token para sacar los comandos de el archivo

        char *contexto_comando;
        char *token_comando = strtok_r(buffer,";\n",&contexto_comando); // Separo por ';' eliminando el '\n'
        
        // While para ver si el largo es correspondiente a si los comandos estan contenidos en el comando del archivo
        while(token_comando != NULL){
            for(int i = 0; i < canticad_cache; i++){
                for(int j = 0; cache_comandos[i][j] != NULL; j++){
                    if(strstr(token_comando, cache_comandos[i][j]) != NULL) comparador_token[i]++;
                    largo_comando_token[i]++;
                }
            }

            bool estado_colocar_comando = true;
            for(int i = 0; i < canticad_cache; i++){
                if(largo_comando_token[i] == comparador_token[i]){
                    estado_colocar_comando = false;
                    break;
                }
            }

            if(estado_colocar_comando){
                char ***temp = (char***)realloc(cache_comandos, sizeof(char**) * (canticad_cache + 1));
                if (temp == NULL) {
                    perror(ROJO "Error en la reasignación de memoria" RESET_COLOR);
                    fclose(file);
                    liberar_memoria_programa();
                    exit(EXIT_FAILURE);
                }
                cache_comandos = temp;

                cache_comandos[canticad_cache] = NULL;

                int elemento = 0;
                char *contexto_elemento;
                char *token_elementos = strtok_r(token_comando," ",&contexto_elemento);

                while(token_elementos != NULL){
                    char **temp_cmd = (char**)realloc(cache_comandos[canticad_cache], sizeof(char*) * (elemento + 1));
                    if (temp_cmd == NULL) {
                        perror(ROJO "Error en la reasignación de memoria" RESET_COLOR);
                        fclose(file);
                        liberar_comandos();
                        exit(EXIT_FAILURE);
                    }
                    cache_comandos[canticad_cache] = temp_cmd;
                    cache_comandos[canticad_cache][elemento++] = strdup(token_elementos);
                    token_elementos = strtok_r(NULL," ",&contexto_elemento);
                }
                cache_comandos[canticad_cache] = (char**)realloc(cache_comandos[canticad_cache], sizeof(char*) * (elemento + 1));
                cache_comandos[canticad_cache][elemento] = NULL;

                canticad_cache++;
                
            }
            // Cambio de comando
            token_comando = strtok_r(NULL,";\n",&contexto_comando);
            
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

    char ***aux = (char***)malloc((tam_nuevo+1)  * sizeof(char**));

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
            int cantidad_elementos = 0;
            while(cache_comandos[contador][cantidad_elementos] != NULL) {
                aux[aux_index] = (char **)realloc(aux[aux_index], (cantidad_elementos + 1) * sizeof(char*));
                if(aux[aux_index] == NULL) {
                    printf("Error al reservar memoria");
                    liberar_memoria_programa();
                    exit(EXIT_FAILURE);
                }
                aux[aux_index][cantidad_elementos] = strdup(cache_comandos[contador][cantidad_elementos]);
                cantidad_elementos++;
            }
            aux[aux_index] = (char **)realloc(aux[aux_index], (cantidad_elementos + 1) * sizeof(char*));
            aux[aux_index][cantidad_elementos] = NULL;
            aux_index++;
        }
    }
    aux[tam_nuevo] = NULL;

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
            liberar_memoria_programa();
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
        archivo_favs = strdup("favs.csv");
    }
    else{
        archivo_favs = (char*)malloc(256 * sizeof(char));  // Reserva espacio para 256 caracteres
        if (archivo_favs == NULL) {
            perror(ROJO "Error al asignar memoria para archivo_favs" RESET_COLOR);
            liberar_memoria_programa();
            exit(EXIT_FAILURE);
        }
        fscanf(file, "%s", archivo_favs);
        fclose(file);
    }
}
