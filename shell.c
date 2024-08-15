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
    printf("%s%s@SHELL_CUSTOM%s:%s%s%s$ %s",VERDE,usuario_actual,BLANCO,AZUL,ruta_actual,BLANCO,RESET_COLOR);   // Prompt que se muestra al esperar un comando
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

// Manejador de signals
void sig_handler(int sig) {
    if(sig == SIGTERM){
        printf(BLANCO "\nSaliendo de la SHELL\n" RESET_COLOR);
        exit(0);
    }
    else if(sig == SIGINT){}
    else if(sig == SIGCHLD){
        wait(NULL); // Espera al proceso hijo que ha terminado
        //printf(BLANCO "\nProceso hijo terminado\n" RESET_COLOR);
    }

}

void liberar_comandos(char **comandos){
    for (int i = 0; comandos[i] != NULL; i++) free(comandos[i]);
    free(comandos);
}

// Manejar comandos internos propios de esta SHELL
int Manejar_comandos_internos(char **comandos){
    if(comandos[1] == NULL && strcmp(comandos[0], "exit") == 0){     //Si se escribe "exit" se termina ded ejecutar el programa
        liberar_comandos(comandos);
        raise(SIGTERM);
    }

    if(strcmp(comandos[0], "cd") == 0){
        if(comandos[1] == NULL) printf(ROJO "FALTA UN ARGUMENTO" RESET_COLOR "\n");
        else if (strcmp(comandos[1], "~") == 0) {   //Implementacion del comando para dirigirse al Directorio Raiz
            char *home_dir = getenv("HOME");
            if (home_dir != NULL) {
                if (chdir(home_dir) != 0) printf(ROJO "Error al ingresar al Directorio HOME" RESET_COLOR "\n");
            }
            else printf(ROJO "Variable de entorno HOME no está definida" RESET_COLOR "\n");
        }
        else if (chdir(comandos[1]) != 0) perror(ROJO "Error al ingresar al Directorio" RESET_COLOR);
        
        liberar_comandos(comandos);
        return 1;
    }
    return 0;
}

void Manejar_comandos_externos(char **comandos){
    pid_t child_pid = fork();

    // Proceso hijo
    if(child_pid == 0){
        if (execvp(comandos[0], comandos) == -1) perror(ROJO "Error en execvp");
        exit(EXIT_FAILURE);
    }
    // Proceso padre
    else if (child_pid > 0) waitpid(child_pid, NULL, 0);
    else {
        perror(ROJO "Error al crear proceso hijo" RESET_COLOR);
        exit(EXIT_FAILURE);
    }
}

int main(){
    signal(SIGINT, sig_handler);
    signal(SIGCHLD, sig_handler);
    signal(SIGTERM, sig_handler);

    while(1){
        char** comandos = entrada_comandos();
        if(comandos == NULL) continue;

        char ***comandos_separados = Identificador_de_comandos(comandos);

        for(int i=0;comandos_separados[i] != NULL;i++){
            if(Manejar_comandos_internos(comandos_separados[i])) continue;   // Si se ejecuto un comando interno que no trate de ejecutar uno externo 

            Manejar_comandos_externos(comandos_separados[i]);
        }

        liberar_comandos(comandos);
    }

    return 0;
}