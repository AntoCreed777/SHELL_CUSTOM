#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "comandos.h"
#include "interfaz.h"
#include "colores.h"

char ***entrada_comandos(){
    char *ruta_actual = directorio_actual();
    char *usuario = usuario_actual();
    printf("%s%s@SHELL_CUSTOM%s:%s%s%s$ %s",VERDE,usuario,BLANCO,AZUL,ruta_actual,BLANCO,RESET_COLOR);   // Prompt que se muestra al esperar un comando
    free(ruta_actual);
    free(usuario);

    //Entrada de linea de comando
    size_t numero_bytes = 0;    //Tamano del buffer
    char *cadena = NULL;
    int buffer_leido = getline(&cadena, &numero_bytes, stdin);

    if (buffer_leido == -1){
        printf(ROJO "ERROR AL LEER INPUT" RESET_COLOR);
        return NULL;
    }
    else if(buffer_leido == 1) return NULL; //Si no se ingresa ningun comando, (solamente ingresa '\n')

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


void pipeling(char **comando,int posicion_pipeling){
    printf("%sComando en Construccion\n%s",AMARILLO,RESET_COLOR);
}


void manejar_comandos_externos(char **comando){
    pid_t child_pid = fork();

    // Proceso hijo
    if(child_pid == 0){
        if (execvp(comando[0], comando) == -1) perror(ROJO "Error en execvp");
        liberar_comandos_anteriores();
        liberar_comandos();
        exit(EXIT_FAILURE);
    }
    // Proceso padre
    else if (child_pid > 0) waitpid(child_pid, NULL, 0);
    else {
        perror(ROJO "Error al crear proceso hijo" RESET_COLOR);
        liberar_comandos_anteriores();
        liberar_comandos();
        exit(EXIT_FAILURE);
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
    
    for(int i=1;comando[i] != NULL;i++) if(strcmp(comando[i],"|") == 0) {pipeling(comando,i);return 1;}

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

