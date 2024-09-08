#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include "interfaz.h"
#include "constantes.h"
#include "memoria_comandos.h"
#define BUFFER_SIZE 1024

char *usuario_actual(){
    FILE *fp = popen("whoami","r");
    
    if (fp == NULL) return strdup(ROJO"ERROR"RESET_COLOR);

    // Leer la salida del comando
    char buffer[BUFFER_SIZE];
    char *usuario = NULL;
    if (fgets(buffer, sizeof(buffer), fp) != NULL) {
        buffer[strcspn(buffer, "\n")] = '\0';   //Busca el salto de linea y lo reemplazo por fin de linea
        usuario = strdup(buffer);
    }

    pclose(fp);
    return usuario;
}


void mostrar_prompt(){
    char *ruta_actual = getcwd(NULL, 0);
    char *usuario = usuario_actual();
    printf("%s%s@SHELL_CUSTOM%s:%s%s%s$ %s", VERDE, usuario, BLANCO, AZUL, ruta_actual, BLANCO, RESET_COLOR); // Prompt que se muestra al esperar un comando
    free(ruta_actual);
    free(usuario);
}

