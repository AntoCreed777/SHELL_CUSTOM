#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>

#define BUFFER_SIZE 1024

#define ROJO        "\033[0;31m"
#define VERDE       "\033[0;32m"
#define AMARILLO    "\033[0;33m"
#define AZUL        "\033[0;34m"
#define MAGENTA     "\033[1;35m"
#define CIAN        "\033[1;36m"
#define BLANCO      "\033[1;37m"
#define RESET_COLOR "\033[0m"

char *Directorio_actual(){
    FILE *fp = popen("pwd","r");

    if (fp == NULL) return NULL;

    // Leer la salida del comando
    char buffer[BUFFER_SIZE];
    if (fgets(buffer, sizeof(buffer), fp) != NULL) {
        buffer[strcspn(buffer, "\n")] = '\0';   //Busca el salto de linea y lo reemplazo por fin de linea
        
        char *ruta_actual = strdup(buffer);
        if (ruta_actual == NULL) return NULL;

        return ruta_actual;
    }
    else return NULL;
}

char *Usuario_actual(){
    FILE *fp = popen("whoami","r");

    if (fp == NULL) return NULL;

    // Leer la salida del comando
    char buffer[BUFFER_SIZE];
    if (fgets(buffer, sizeof(buffer), fp) != NULL) {
        buffer[strcspn(buffer, "\n")] = '\0';   //Busca el salto de linea y lo reemplazo por fin de linea
        
        char *usuario_actual = strdup(buffer);
        if (usuario_actual == NULL) return NULL;

        return usuario_actual;
    }
    else return NULL;
}

char **entrada_comandos(){
    char *ruta_actual = Directorio_actual();
    char *usuario_actual = Usuario_actual();
    printf("%s%s@SHELL_CUSTOM:%s%s$ %s",VERDE,usuario_actual,AZUL,ruta_actual, RESET_COLOR);   // Prompt que se muestra al esperar un comando
    free(ruta_actual);
    free(usuario_actual);

    //Entrada de linea de comando
    size_t numero_bytes = 0;    //Tamano del buffer
    char *cadena = NULL;
    int buffer_leido = getline(&cadena, &numero_bytes, stdin);

    if (buffer_leido == -1){
        printf(ROJO "ERROR" RESET_COLOR);
        return NULL;
    }
    else if(buffer_leido == 1) return NULL; //Si no se ingresa ningun comando, (solamente ingresa '\n')

    //Extraccion de las distintas partes del comando
    char **comandos = NULL;
    int comando = 0;

    char *token = strtok(cadena," \n"); // Separo por ' ' (espacio) eliminando el '\n'

    while(token != NULL){
        comandos = (char**)realloc(comandos,sizeof(char*) * (comando+1));

        if(comandos == NULL){
            perror(ROJO "Error en la reasignación de memoria");
            exit(EXIT_FAILURE);
        }

        comandos[comando] = (char*)malloc(sizeof(char) * (strlen(token) + 1));
        strcpy(comandos[comando],token);

        comando++;
        token = strtok(NULL, " \n");
    }
    
    // Añadimos un NULL al final del array para marcar el fin de los comandos
    comandos = (char **)realloc(comandos, sizeof(char*) * (comando + 1));
    comandos[comando] = NULL;

    free(cadena); // Liberar la cadena original

    return comandos;
}

// Manejador de signals
void sig_handler(int sig) {
    if(sig == SIGINT || sig == SIGTERM){
        printf(BLANCO "\nSaliendo de la SHELL\n" RESET_COLOR);
        exit(0);
    }
    else if(sig == SIGCHLD){
        wait(NULL); // Espera al proceso hijo que ha terminado
        //printf(BLANCO "\nProceso hijo terminado\n" RESET_COLOR);
    }

}

void liberar_comandos(char **comandos){
    for (int i = 0; comandos[i] != NULL; i++) free(comandos[i]);
    free(comandos);
}

char ***Identificador_de_comandos(char **comandos){
    char ***identificados = NULL;
    int contador_comandos=0;
    int contador_elementos=1;

    for(int i=0;comandos[i] != NULL;i++){
        if(comandos[i][0] != '-'){
            // Nuevo Comando
            contador_comandos++;
            contador_elementos = 1;
            
            // Espacio para nuevo comando
            identificados = (char***)realloc(identificados,sizeof(char**) * contador_comandos);

            // Asignar memoria para el primer elemento del comando
            identificados[contador_comandos-1] = (char**)malloc(sizeof(char*) * contador_elementos);
            identificados[contador_comandos-1][contador_elementos-1] = comandos[i];
        }
        else{
            // Nuevo elemento del comando
            contador_elementos++;

            // Asignar memoria para el nuevo elemento del comando
            identificados[contador_comandos-1] = (char**)realloc(identificados[contador_comandos-1],sizeof(char*) * contador_elementos);
            identificados[contador_comandos-1][contador_elementos-1] = comandos[i];
        }
        //printf("%s\n",identificados[contador_comandos-1][contador_elementos-1]);
    }

    // Añadir un NULL para indicar el final de la lista de comandos
    identificados = (char***)realloc(identificados, sizeof(char**) * (contador_comandos + 1));
    identificados[contador_comandos] = NULL;

    return identificados;
}

int main(){
    signal(SIGINT, sig_handler);
    signal(SIGCHLD, sig_handler);
    signal(SIGTERM, sig_handler);


    while(1){
        char** comandos = entrada_comandos();
        if(comandos == NULL){
            printf(ROJO "NO SE INGRESO NADA\n");
            continue;
        }

        char ***conjunto_comandos = Identificador_de_comandos(comandos);

        for(int i=0;conjunto_comandos[i] != NULL;i++){
            for(int j=0;conjunto_comandos[i][j] != NULL;j++) printf("%s\t",conjunto_comandos[i][j]);
            // printf("%s",conjunto_comandos[i][0]);
            printf("\n");
        }

        liberar_comandos(comandos);
    }

    return 0;
}