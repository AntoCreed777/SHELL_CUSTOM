#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include "colores.h"
#include "interfaz.h"
#include "signal_handlers.h"

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



int main(){
    signal(SIGINT, sigint_handler);
    signal(SIGCHLD, sigchld_handler);
    signal(SIGTERM, sigterm_handler);

    while(1){
        comandos = entrada_comandos();
        if(comandos == NULL) continue;

        for(int i=0;comandos[i] != NULL;i++){
            if(manejar_comandos_internos(comandos[i])) continue;   // Si se ejecuto un comando interno que no trate de ejecutar uno externo 

            manejar_comandos_externos(comandos[i]);
        }

        guardar_comandos();
        liberar_comandos();
    }

    return 0;
}