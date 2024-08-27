#include <stdio.h>
#include <stdlib.h>
#include "interfaz.h"
#include "colores.h"
#define BUFFER_SIZE 1024



char ***comandos = NULL;
char ***comandos_anteriores = NULL;

char *directorio_actual(){
    FILE *fp = popen("pwd","r");

    if (fp == NULL) return NULL;

    // Leer la salida del comando
    char buffer[BUFFER_SIZE];
    char *ruta_actual = NULL;
    if (fgets(buffer, sizeof(buffer), fp) != NULL) {
        buffer[strcspn(buffer, "\n")] = '\0';   //Busca el salto de linea y lo reemplazo por fin de linea
        ruta_actual = strdup(buffer);
    }
    
    pclose(fp);
    return ruta_actual;
}

char *usuario_actual(){
    FILE *fp = popen("whoami","r");
    
    if (fp == NULL) return NULL;

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
