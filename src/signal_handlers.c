#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include "signal_handlers.h"
#include "interfaz.h"
#include "comandos.h"

//A continuacion se detallan las diferentes funciones para manejar las distintas señales
void sigterm_handler(int sig) {
    printf(BLANCO "\nSaliendo de la SHELL\n" RESET_COLOR);
    liberar_comandos();
    liberar_comandos_anteriores();
    exit(0);
}

void sigint_handler(int sig) {
    if(kill(0,SIGKILL) < 0){
        printf(ROJO "Ocurrio un error al matar a procesos hijos" RESET_COLOR);
        liberar_comandos();
        liberar_comandos_anteriores();
        exit(EXIT_FAILURE);
    }
}

void sigchld_handler(int sig) {
    // Manejar a todos los procesos HIJOS que terminaron sin bloquear al proceso PADRE
    while (waitpid(-1, NULL, WNOHANG) > 0);
}
