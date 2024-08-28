#include <stdio.h>
#include <signal.h>
#include "signal_handlers.h"
#include "interfaz.h"
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

void sig_handler(int sig) {
    if(sig == SIGTERM){
        printf(BLANCO "\nSaliendo de la SHELL\n" RESET_COLOR);
        liberar_comandos();
        liberar_comandos_anteriores();
        exit(0);
    }
    else if(sig == SIGINT){}
    else if(sig == SIGCHLD){
        wait(NULL); // Espera al proceso hijo que ha terminado
        //printf(BLANCO "\nProceso hijo terminado\n" RESET_COLOR);
    }

}


//A continuacion se detallan las diferentes funciones para manejar las distintas señales
void sigterm_handler(int sig) {
    printf(BLANCO "\nSaliendo de la SHELL\n" RESET_COLOR);
    liberar_comandos();
    liberar_comandos_anteriores();
    exit(0);
}

void sigint_handler(int sig) {
}

void sigchld_handler(int sig) {
    wait(NULL);
}


