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

void liberar_comandos_anteriores(){
    if(comandos_anteriores == NULL) return;
    for(int i=0;comandos_anteriores[i] != NULL;i++) {
        for(int j=0;comandos_anteriores[i][j] != NULL;j++) free(comandos_anteriores[i][j]);
        free(comandos_anteriores[i]);
    }
    free(comandos_anteriores);
}

void manejar_comandos_externos(char **comando){
    int num_pipe = 0;
    int i = 0;
    
    // Contar el número de pipes
    while (comando[i] != NULL) {
        if (strcmp(comando[i], "|") == 0) {
            num_pipe++;
        }
        i++;
    }

    // Caso sin pipes
    if (num_pipe == 0) {
        pid_t c_pid = fork();

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
        while (comando[i] != NULL && strcmp(comando[i], "|") != 0) {
            comando_actual[contador_argumento] = comando[i];
            i++, contador_argumento++;
        }
        comando_actual[contador_argumento] = NULL;
        
        // Saltar el pipe si existe
        if (comando[i] != NULL && strcmp(comando[i], "|") == 0) i++;

        pid_t pid = fork();
        if (pid == 0) {
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
        } else if (pid < 0) {
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
    if(comando[1] == NULL && strcmp(comando[0], "exit") == 0){     //Si se escribe "exit" se termina ded ejecutar el programa
        raise(SIGTERM);
    }

    if(strcmp(comando[0], "!!") == 0){
        if(comandos_anteriores == NULL) return 1;

        //Impresion de los comandos anteriores
        for(int i=0;comandos_anteriores[i] != NULL;i++) {
            printf("Command %d: ",i+1);
            for(int j=0;comandos_anteriores[i][j] != NULL;j++) printf("%s ",comandos_anteriores[i][j]);
            printf("\n");
        }

        //Ejecucion de los comandos anteriores

        for(int i=0;comandos_anteriores[i] != NULL;i++) manejar_comandos_externos(comandos_anteriores[i]);

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
                printf("\n" AMARILLO "ALARMA" RESET_COLOR ":" MAGENTA " %s" RESET_COLOR "\n", comando[3]);
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

    return 0;   //No encontro ningun Comando Interno
}

void guardar_comandos(){
    if(strcmp(comandos[0][0],"!!") == 0) return;
    
    if(comandos_anteriores != NULL) liberar_comandos_anteriores();
    comandos_anteriores = NULL;

    int num_comandos = 0;

    while(comandos[num_comandos] != NULL) {   // Comandos

        comandos_anteriores = (char***)realloc(comandos_anteriores,sizeof(char**) * (num_comandos+1));
        
        if(comandos_anteriores == NULL){
            perror(ROJO "Error en la reasignación de memoria");
            liberar_comandos();
            exit(EXIT_FAILURE);
        }

        comandos_anteriores[num_comandos]= NULL;

        int num_elementos = 0;

        while(comandos[num_comandos][num_elementos] != NULL){    // Elementos del Comando
            comandos_anteriores[num_comandos] = (char**)realloc(comandos_anteriores[num_comandos],sizeof(char*) * (num_elementos+1));
            
            if(comandos_anteriores[num_comandos] == NULL){
                perror(ROJO "Error en la reasignación de memoria");
                exit(EXIT_FAILURE);
            }

            comandos_anteriores[num_comandos][num_elementos] = strdup(comandos[num_comandos][num_elementos]);
            num_elementos++;
        }
        comandos_anteriores[num_comandos] = (char**)realloc(comandos_anteriores[num_comandos],sizeof(char*) * (num_elementos+1));
        comandos_anteriores[num_comandos][num_elementos] = NULL;
        num_comandos++;
    }
    comandos_anteriores = (char***)realloc(comandos_anteriores,sizeof(char**) * (num_comandos+1));
    comandos_anteriores[num_comandos] = NULL;
}

