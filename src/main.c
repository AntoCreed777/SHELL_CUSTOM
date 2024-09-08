#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include "constantes.h"
#include "interfaz.h"
#include "signal_handlers.h"
#include "comandos.h"
#include "favs.h"


int main(){
    signal(SIGINT, sigint_handler);
    signal(SIGCHLD, sigchld_handler);
    signal(SIGTERM, sigterm_handler);

    cargar_ruta_favs();

    while(1){
        comandos = entrada_comandos();
        if(comandos == NULL) continue;

        for(int i=0;comandos[i] != NULL;i++){
            if(manejar_comandos_internos(comandos[i], i)) continue;   // Si se ejecuto un comando interno que no trate de ejecutar uno externo 

            manejar_comandos_externos(comandos[i], i);
        }

        guardar_comandos_anteriores();
        guardar_comandos_cache();
        liberar_comandos();
    }

    return 0;
}