#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include "memoria_comandos.h"
#include "externos_comandos.h"

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
