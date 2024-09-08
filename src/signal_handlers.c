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
    liberar_memoria_programa();
    exit(0);
}

void sigint_handler(int sig) {
    if(c_pid == -1) return;

    if (kill(c_pid, SIGKILL) < 0) {  // Matar todos los procesos en el grupo de procesos actual
        perror(ROJO "Ocurrió un error al matar a los procesos hijos" RESET_COLOR);
        liberar_memoria_programa();
        exit(EXIT_FAILURE);
    }
}

void sigchld_handler(int sig) {
    // Manejar a todos los procesos HIJOS que terminaron sin bloquear al proceso PADRE
    while (waitpid(-1, NULL, WNOHANG) > 0);
    c_pid = -1;
}
