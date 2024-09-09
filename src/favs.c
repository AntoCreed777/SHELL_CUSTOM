#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include "favs.h"
#include "constantes.h"
#include "memoria_comandos.h"
#include "comandos.h"
#include "interfaz.h"


void guardar_favs(){    
    if (archivo_favs == NULL || access(archivo_favs, F_OK) == -1) {
        printf(AMARILLO "El archivo de favoritos no existe\n" RESET_COLOR);
        return;
    }
    
    FILE *file = fopen(archivo_favs,"w");
    if (file == NULL) {
        perror(ROJO "Error al abrir el archivo" RESET_COLOR);
        return;
    }

    if(cache_comandos == NULL || cache_comandos[0] == NULL){
        printf(AMARILLO "No hay comandos favoritos para guardar\n" RESET_COLOR);
        fclose(file);
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
    if (cache_comandos == NULL || cache_comandos[0] == NULL){
        printf(AMARILLO "No hay comandos favoritos\n" RESET_COLOR);
        return;
    }

    int i = 0;
    printf(CIAN "Comandos Favoritos:\n" RESET_COLOR);
    while(cache_comandos[i] != NULL){
        int j = 0;
        printf(AZUL "%d:  ", i + 1);
        while (cache_comandos[i][j] != NULL){
            printf(ROJO "%s " RESET_COLOR, cache_comandos[i][j]);
            j++; 
        }
        printf("\n");
        i++;
    }
    return;
}

bool cargar_favs(){
    if (archivo_favs == NULL || access(archivo_favs, F_OK) == -1) {
        printf(AMARILLO "El archivo de favoritos no existe\n" RESET_COLOR);
        return false;
    }
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
        // Creamos el token para sacar los comandos de el archivo
        char *contexto_comando;
        char *token_comando = strtok_r(buffer,";\n",&contexto_comando); // Separo por ';' eliminando el '\n'
        
        // While para ver si el largo es correspondiente a si los comandos estan contenidos en el comando del archivo
        while(token_comando != NULL){
            char *token_comando_aux_original = strdup(token_comando);
            bool estado_colocar_comando = true;

            if(cache_comandos != NULL) for(int i=0; cache_comandos[i] != NULL; i++){    // Recorro los comandos en cache
                char *token_comando_aux = strdup(token_comando);
                char *token_argumento = strtok(token_comando_aux, " ");
                int cantidad_elementos = 0, cantidad_encontrada = 0;

                while(token_argumento != NULL){
                    for(int j = 0; cache_comandos[i][j] != NULL; j++){
                        if(strcmp(cache_comandos[i][j], token_argumento) == 0){
                            cantidad_encontrada++;
                            break;
                        }
                    }
                    cantidad_elementos++;
                    token_argumento = strtok(NULL, " ");
                }

                int cantidad_elementos_cache = 0;
                for (int j = 0; cache_comandos[i][j] != NULL; j++) cantidad_elementos_cache++;

                printf("Cantidad de elementos: %d\n", cantidad_elementos);
                printf("Cantidad de elementos encontrados: %d\n", cantidad_encontrada);
                printf("Cantidad de elementos en cache: %d\n", cantidad_elementos_cache);

                if (cantidad_elementos == cantidad_encontrada && cantidad_elementos == cantidad_elementos_cache) {
                    printf("Comando archivo: %s\n", token_comando_aux_original);
                    printf("Comando cache: ");
                    estado_colocar_comando = false;
                    free(token_comando_aux);
                    break;
                }
                free(token_comando_aux);
            }

            free(token_comando_aux_original);

            if(estado_colocar_comando){
                cache_comandos = (char***)realloc(cache_comandos, sizeof(char**) * (canticad_cache + 1));
                if (cache_comandos == NULL) {
                    perror(ROJO "Error en la reasignación de memoria" RESET_COLOR);
                    fclose(file);
                    liberar_memoria_programa();
                    exit(EXIT_FAILURE);
                }

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

        printf("\n\n\n");
    }

    fclose(file);
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

    if(manejar_comandos_internos(cache_comandos[numero-1], -1)) return;
    manejar_comandos_externos(cache_comandos[numero-1], -1);
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

void cargar_ruta_favs() {
    // Verificar que direccion_favs esté inicializado
    if (direccion_favs == NULL) {
        printf("Direccion de favoritos no inicializada.\n");
        return;
    }

    // Verificar que direccion_favs no sea una cadena vacía
    if (strlen(direccion_favs) == 0) {
        printf("Direccion de favoritos vacía.\n");
        return;
    }

    FILE *file = fopen(direccion_favs, "r");
    if (file == NULL) {
        perror(ROJO "Error al abrir el archivo de dirección de favoritos" RESET_COLOR);
        printf(AMARILLO "No existe archivo de guardado de favoritos.\nDebera crear uno con el comando: " CIAN "favs crear $(ruta)\n" RESET_COLOR);
        return;
    }

    // Asignar un buffer temporal para leer el contenido
    char buffer[BUFFER_SIZE];

    if (fgets(buffer, sizeof(buffer), file) != NULL) {
        buffer[strcspn(buffer, "\n")] = '\0';

        // Liberar memoria previa y asignar nueva memoria para archivo_favs
        if (archivo_favs != NULL) free(archivo_favs);

        archivo_favs = strdup(buffer);
        if (archivo_favs == NULL) {
            perror("Error al asignar memoria para archivo_favs");
            fclose(file);
            return;
        }
    }
    else printf(AMARILLO "No se encontró ninguna ruta de archivo de favoritos\n" RESET_COLOR);

    fclose(file);
}
