#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <stdbool.h>
#include "memoria_comandos.h"
#include "externos_comandos.h"
#include "constantes.h"

void manejar_comandos_externos(char **comando, int num_comando){
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
            perror(ROJO "execvp" RESET_COLOR);  // Se muestra solo si execvp falla
            liberar_memoria_programa();
            exit(EXIT_FAILURE);  // Terminar si execvp falla
        }
        else if (c_pid > 0) {
            int status;
            
            if (waitpid(c_pid, &status, 0) == -1) {
                perror(ROJO "waitpid" RESET_COLOR);
                /*
                liberar_memoria_programa();
                exit(EXIT_FAILURE);
                */
               comandos_validos[num_comando] = false;
               return;
            }

            if(num_comando == -1) return;

            if (WIFEXITED(status)) {
                if (WEXITSTATUS(status) == 0) comandos_validos[num_comando] = true;
                else comandos_validos[num_comando] = false;
            }
            else if (WIFSIGNALED(status))
                comandos_validos[num_comando] = false;

        } else {
            perror(ROJO "fork" RESET_COLOR);
            liberar_memoria_programa();
            exit(EXIT_FAILURE);
        }
        return;
    }

    // Crear pipes
    int pidfc[num_pipe][2];
    for (int j = 0; j < num_pipe; j++) {
        if (pipe(pidfc[j]) == -1) {
            perror(ROJO "pipe" RESET_COLOR);
            for(int k = 0; k < j; k++) {
                close(pidfc[k][0]);
                close(pidfc[k][1]);
            }
            return;
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
                    perror(ROJO "dup2 input" RESET_COLOR);
                    liberar_memoria_programa();
                    exit(EXIT_FAILURE);
                }
            }
            
            // Redirigir la salida para todos menos el último comando
            if (indice_pipes < num_pipe) {
                if (dup2(pidfc[indice_pipes][1], STDOUT_FILENO) == -1) {
                    perror(ROJO "dup2 output" RESET_COLOR);
                    liberar_memoria_programa();
                    exit(EXIT_FAILURE);
                }
            }
            
            // Cerrar todos los pipes en el proceso hijo
            for (int n = 0; n < num_pipe; n++) {
                close(pidfc[n][0]);
                close(pidfc[n][1]);
            }

            execvp(comando_actual[0], comando_actual);
            perror(ROJO "execvp" RESET_COLOR);  // Se muestra solo si execvp falla
            liberar_memoria_programa();
            exit(EXIT_FAILURE);  // Terminar si execvp falla
        }
        else if (c_pid > 0) {
            // Cerrar el pipe en el proceso padre
            if (indice_pipes > 0) close(pidfc[indice_pipes - 1][0]);
            if (indice_pipes < num_pipe) close(pidfc[indice_pipes][1]);

            int status;
            if (waitpid(c_pid, &status, 0) == -1) {
                perror(ROJO "waitpid" RESET_COLOR);
                for(int k = 0; k < num_pipe; k++) {
                    close(pidfc[k][0]);
                    close(pidfc[k][1]);
                }
                comandos_validos[num_comando] = false;
                return;
            }
            
            if(num_comando == -1) {
                indice_pipes++;
                continue;
            }

            if (WIFEXITED(status)) {
                if (WEXITSTATUS(status) == 0) comandos_validos[num_comando] = true;
                else comandos_validos[num_comando] = false;
            }
            else if (WIFSIGNALED(status))
                comandos_validos[num_comando] = false;
        }
        else if (c_pid < 0) {
            perror(ROJO "fork" RESET_COLOR);
            liberar_memoria_programa();
            exit(EXIT_FAILURE);
        }
        indice_pipes++;
    }
}
