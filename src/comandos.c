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
#include "colores.h"
#include "utils.h"
#define BUFFER_SIZE 1024

pid_t c_pid = -1;
const char *archivo_favs = "favs.csv";
int indice_inicio_anterior_linea_comando = 0;

char ***entrada_comandos(){
    mostrar_prompt();

    //Entrada de linea de comando
    size_t numero_bytes = 0;    //Tamano del buffer
    char *cadena = NULL;
    int buffer_leido = getline(&cadena, &numero_bytes, stdin);

    
    if (buffer_leido == -1){
        printf(ROJO "ERROR AL LEER INPUT" RESET_COLOR);
        return NULL;
    }
    if(is_empty_linea(cadena)) 
        return NULL; //Si no se ingresa ningun comando, (solamente ingresa '\n')

    //Extraccion de las distintas partes del comando
    char ***comandos = NULL;
    int comando = 0;
    char *contexto_comando;

    char *token_comando = strtok_r(cadena,";\n",&contexto_comando); // Separo por ';' eliminando el '\n'

    while(token_comando != NULL){

        comandos = (char***)realloc(comandos,sizeof(char**) * (comando+1));

        if(comandos == NULL){
            perror(ROJO "Error en la reasignación de memoria");
            exit(EXIT_FAILURE);
        }
        comandos[comando] = NULL;

        int elemento = 0;
        char *contexto_elemento;
        char *token_elementos = strtok_r(token_comando," ",&contexto_elemento); // Separo por ' ' (espacio)

        while(token_elementos != NULL){

            comandos[comando] = (char**)realloc(comandos[comando],sizeof(char*) * (elemento+1));

            if(comandos[comando] == NULL){
                perror(ROJO "Error en la reasignación de memoria");
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

    free(cadena); // Liberar la cadena original

    return comandos;
}

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

void manejar_comandos_externos(char **comando){
    int num_pipe = 0;
    int i = 0;
    
    // Contar el número de pipes
    for (i = 0; comando[i] != NULL; i++) 
        if (strcmp(comando[i], "|") == 0) num_pipe++;

    // Caso sin pipes
    if (num_pipe == 0) {
        c_pid = fork();

        if (c_pid == 0) {
            execvp(comando[0], comando);
            perror("execvp");  // Se muestra solo si execvp falla
            exit(EXIT_FAILURE);  // Terminar si execvp falla
        } else if (c_pid > 0) {
            waitpid(c_pid, NULL, 0);
        } else {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        return;
    }

    // Crear pipes
    int pidfc[num_pipe][2];
    for (int j = 0; j < num_pipe; j++) {
        if (pipe(pidfc[j]) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
    }
    
    // Ejecutar comandos con pipes
    i = 0;
    int indice_pipes = 0;  // Índice de pipes

    while (comando[i] != NULL) {
        char *comando_actual[1024] = {NULL};  // Inicializar a NULL
        int contador_argumento = 0;
        
        // Extraer el comando actual hasta el próximo pipe o el final
        while (comando[i] != NULL && strcmp(comando[i], "|") != 0) comando_actual[contador_argumento++] = comando[i++];
        comando_actual[contador_argumento] = NULL;
        
        // Saltar el pipe si existe
        if (comando[i] != NULL && strcmp(comando[i], "|") == 0) i++;

        c_pid = fork();
        if (c_pid == 0) {
            // Redirigir la entrada para todos menos el primer comando
            if (indice_pipes > 0) {
                if (dup2(pidfc[indice_pipes - 1][0], STDIN_FILENO) == -1) {
                    perror("dup2 input");
                    exit(EXIT_FAILURE);
                }
            }
            
            // Redirigir la salida para todos menos el último comando
            if (indice_pipes < num_pipe) {
                if (dup2(pidfc[indice_pipes][1], STDOUT_FILENO) == -1) {
                    perror("dup2 output");
                    exit(EXIT_FAILURE);
                }
            }
            
            // Cerrar todos los pipes en el proceso hijo
            for (int n = 0; n < num_pipe; n++) {
                close(pidfc[n][0]);
                close(pidfc[n][1]);
            }

            execvp(comando_actual[0], comando_actual);
            perror("execvp");  // Se muestra solo si execvp falla
            exit(EXIT_FAILURE);  // Terminar si execvp falla
        } else if (c_pid < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        indice_pipes++;
    }

    // Proceso padre cierra todos los descriptores de pipes
    for (int indice_pipes = 0; indice_pipes < num_pipe; indice_pipes++) {
        close(pidfc[indice_pipes][0]);
        close(pidfc[indice_pipes][1]);
    }

    // Esperar a que todos los procesos hijos terminen
    for (int j = 0; j <= num_pipe; j++) {
        wait(NULL);
    }
}

// Manejar comandos internos propios de esta SHELL
int manejar_comandos_internos(char **comando){
    if(strcmp(comando[0], "exit") == 0){     //Si se escribe "exit" se termina ded ejecutar el programa
        raise(SIGTERM);
    }

    if(strcmp(comando[0], "!!") == 0){
        if(cache_comandos == NULL) return 1;

        //Impresion de los comandos anteriores
        for(int i=indice_inicio_anterior_linea_comando,k=0;cache_comandos[i] != NULL;i++,k++) {
            printf(AZUL "Command %d: " RESET_COLOR,k+1);
            for(int j=0;cache_comandos[i][j] != NULL;j++) printf(ROJO "%s " RESET_COLOR,cache_comandos[i][j]);
            printf("\n");
        }

        //Ejecucion de los comandos anteriores

        for(int i=indice_inicio_anterior_linea_comando;cache_comandos[i] != NULL;i++)
            manejar_comandos_externos(cache_comandos[i]);

        return 1;
    }

    if(strcmp(comando[0], "cd") == 0){
        if(comando[1] == NULL) printf(ROJO "FALTA UN ARGUMENTO" RESET_COLOR "\n");
        else if (strcmp(comando[1], "~") == 0) {   //Implementacion del comando para dirigirse al Directorio Raiz
            char *home_dir = getenv("HOME");
            if (home_dir != NULL) {
                if (chdir(home_dir) != 0) printf(ROJO "Error al ingresar al Directorio HOME" RESET_COLOR "\n");
            }
            else printf(ROJO "Variable de entorno HOME no está definida" RESET_COLOR "\n");
        }
        else if (chdir(comando[1]) != 0) perror(ROJO "Error al ingresar al Directorio" RESET_COLOR);
        
        return 1;
    }

    if(strcmp(comando[0], "set") == 0){
        if(comando[1] == NULL || comando[2] == NULL) printf(ROJO "FALTAN ARGMENTOS" RESET_COLOR "\n");
        
        else if(strcmp(comando[2], "help") == 0) printf("set recordatorio [time] [mensaje]\n"); //Ayuda de como es la estructura del comando

        else if(comando[3] == NULL) printf(ROJO "FALTAN ARGMENTOS" RESET_COLOR "\n");

        else if(!is_number(comando[2])) printf(ROJO "DEBE SER UN NUMERO PARA INDICAR TIEMPO" RESET_COLOR "\n");

        else{
            pid_t pid = fork();

            if (pid == 0) {     //Proceso hijo
                sleep(atoi(comando[2]));
                printf("\n" AMARILLO "ALARMA" RESET_COLOR ": " MAGENTA);
                int contador = 3;
                while (comando[contador] != NULL) printf("%s ", comando[contador++]);
                printf("\n");
                mostrar_prompt();
                exit(0);
            }
            else if (pid < 0) {
                perror("Error al crear el proceso hijo");
                exit(1);
            }

            printf(AMARILLO"Alarma configurada para dentro de %d segundos."RESET_COLOR"\n"RESET_COLOR, atoi(comando[2]));
        }

        return 1;
    }

    if(strcmp(comando[0], "favs") == 0){
        if(comando[1] == NULL) printf(ROJO "FALTAN ARGMENTOS" RESET_COLOR "\n");

        else if(strcmp(comando[1], "help") == 0){
            printf(AMARILLO "Lista de argumentos disponibles:\n"
            AZUL " crear " ROJO " ruta/ejemplo.csv\n"
            AZUL " mostrar\n" " eliminar " ROJO "num1,num2\n"
            AZUL " buscar " ROJO "cmd\n"
            AZUL " borrar\n"
            AZUL " ejecutar " ROJO "num\n"
            AZUL " cargar\n"
            AZUL " guardar\n"
            RESET_COLOR);
        }

        else if(strcmp(comando[1], "crear") == 0){    //favs mostrar (despliega la lista comandos existentes en la lista con su respectivo número)
            if(comando[2] == NULL) {
                printf(ROJO "FALTAN ARGMENTOS" RESET_COLOR "\n");
                return 1;
            }
        }


        else if(strcmp(comando[1], "mostrar") == 0){    //favs mostrar (despliega la lista comandos existentes en la lista con su respectivo número)
            mostrar_favs();
        }

        else if(strcmp(comando[1], "eliminar") == 0){   //favs eliminar num1,num2 (Eliminar comandos asociados a los números entregados entrecomas)
            if(comando[2] == NULL) {
                printf(ROJO "FALTAN ARGMENTOS" RESET_COLOR "\n");
                return 1;
            }
            int numero_elimminar = atoi(comando[2]);
            eliminar_favs(numero_elimminar);
        }

        else if(strcmp(comando[1], "buscar") == 0){     //favs buscar cmd (Busca comandos que contengan substring cmd en la lista de favoritos y los despliega en pantalla junto con su número asociado)
            if(comando[2] == NULL) {
                printf(ROJO "FALTAN ARGMENTOS" RESET_COLOR "\n");
                return 1;
            }
            buscar_favs(comando[2]);
        }

        else if(strcmp(comando[1], "borrar") == 0){     //favs borrar (Borra todos los comandos en la lista de favoritos)
            borrar_favs();
        }

        else if(strcmp(comando[1], "ejecutar") == 0){   //favs num ejecutar (Ejecuta el comando, cuyo número en la lista es num) //INVERTI EL ORDEN DE LOS ARGUMENTOS
            if(comando[2] == NULL) {
                printf(ROJO "FALTAN ARGMENTOS" RESET_COLOR "\n");
                return 1;
            }


        }

        else if(strcmp(comando[1], "cargar") == 0){     //favs cargar (Lee comandos de archivo de favoritos, los mantiene en memoria y los despliega en pantalla)
            cargar_favs();
            mostrar_favs();
        }

        else if(strcmp(comando[1], "guardar") == 0){    //favs guardar (Guarda comandos agregados en sesión de shell actual)
            guardar_favs();
        }

        return 1;
    }

    return 0;   //No encontro ningun Comando Interno
}

void guardar_comandos(){
    int num_comandos = 0, num_invalidos = 0, num_cache = 0;

    if (cache_comandos != NULL)
        for(int i=0;cache_comandos[i] != NULL;i++) 
            num_cache++;

    int old = indice_inicio_anterior_linea_comando;
    indice_inicio_anterior_linea_comando = num_cache;

    // Procesar los nuevos comandos para guardarlos
    while(comandos[num_comandos + num_invalidos] != NULL) {
        if(strcmp(comandos[num_comandos + num_invalidos][0],"!!") == 0 || strcmp(comandos[num_comandos + num_invalidos][0],"favs") == 0) {
            if(num_comandos == 0)
                indice_inicio_anterior_linea_comando = old;
            else
                indice_inicio_anterior_linea_comando--;
            
            num_invalidos++;
            continue;
        }

        int posicion = num_cache + num_comandos;

        // Reasignar memoria para el nuevo comando
        cache_comandos = (char***)realloc(cache_comandos,sizeof(char**) * (posicion + 1));
        if(cache_comandos == NULL){
            perror(ROJO "Error en la reasignación de memoria");
            liberar_comandos();
            exit(EXIT_FAILURE);
        }

        cache_comandos[posicion] = NULL;

        int num_elementos = 0;
        
        // Copiar los elementos del comando
        while(comandos[num_comandos + num_invalidos][num_elementos] != NULL){    // Elementos del Comando
            cache_comandos[posicion] = (char**)realloc(cache_comandos[posicion],sizeof(char*) * (num_elementos+1));
            
            if(cache_comandos[posicion] == NULL){
                perror(ROJO "Error en la reasignación de memoria");
                exit(EXIT_FAILURE);
            }

            cache_comandos[posicion][num_elementos] = strdup(comandos[num_comandos + num_invalidos][num_elementos]);
            num_elementos++;
        }
        // Terminar el array de elementos del comando con NULL
        cache_comandos[posicion] = (char**)realloc(cache_comandos[posicion],sizeof(char*) * (num_elementos+1));
        cache_comandos[posicion][num_elementos] = NULL;
        num_comandos++;
    }

    // Terminar el array de comandos anteriores con NULL
    cache_comandos = (char***)realloc(cache_comandos,sizeof(char**) * (num_comandos + num_cache +1));
    cache_comandos[num_comandos + num_cache] = NULL;
}

void guardar_favs(){
    FILE *file = fopen(archivo_favs,"a+");
    if (file == NULL) {
        perror("Error al abrir el archivo");
        liberar_comandos();
        liberar_cache();
        exit(EXIT_FAILURE);
    }

    
    for(int i=0;cache_comandos[i] != NULL;i++){
        for(int j=0;cache_comandos[i][j] != NULL;j++){
            fprintf(file,"%s ",cache_comandos[i][j]);
        }
        fprintf(file,";\n");
    }

    fclose(file);
}

void borrar_favs(){
    if(access(archivo_favs, F_OK) != 0) return; //Si no existe el archivo

    if (remove(archivo_favs) != 0){
        perror("Error al eliminar el archivo");
        exit(EXIT_FAILURE);
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
    printf("COMANDO EN CONSTRUCCION\n");
}

void eliminar_favs(int numero){
    if(numero <= 0){
        perror("Número invalido.");
        return;
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
    while(fgets(buffer, sizeof(buffer), origenFile)){
        if(contador++ != numero){
            fputs(buffer, origen_temporal);
        }
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
            printf(AZUL"%d: " ROJO "%s\n" RESET_COLOR, contador++, buffer);
        }
    }
}