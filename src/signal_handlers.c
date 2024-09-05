#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include "signal_handlers.h"
#include "interfaz.h"
#include "comandos.h"

extern pid_t c_pid;

//A continuacion se detallan las diferentes funciones para manejar las distintas señales
void sigterm_handler(int sig) {
    printf(BLANCO "\nSaliendo de la SHELL\n" RESET_COLOR);
    liberar_comandos();
    liberar_cache();
    exit(0);
}

void sigint_handler(int sig) {
    if(c_pid == -1) return;

    pid_t pgrp = getpgrp();  // Obtener el ID del grupo de procesos

    if (kill(c_pid, SIGKILL) < 0) {  // Matar todos los procesos en el grupo de procesos actual
        perror("Ocurrió un error al matar a los procesos hijos");
        liberar_comandos();
        liberar_cache();
        exit(EXIT_FAILURE);
    }
}

void sigchld_handler(int sig) {
    // Manejar a todos los procesos HIJOS que terminaron sin bloquear al proceso PADRE
    while (waitpid(-1, NULL, WNOHANG) > 0);
    c_pid = -1;
}
