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

char ***comandos = NULL;
char ***comandos_anteriores = NULL;

char *Directorio_actual(){
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

char *Usuario_actual(){
    FILE *fp = popen("whoami","r");
    
    if (fp == NULL) return NULL;

    // Leer la salida del comando
    char buffer[BUFFER_SIZE];
    char *usuario_actual = NULL;
    if (fgets(buffer, sizeof(buffer), fp) != NULL) {
        buffer[strcspn(buffer, "\n")] = '\0';   //Busca el salto de linea y lo reemplazo por fin de linea
        usuario_actual = strdup(buffer);
    }

    pclose(fp);
    return usuario_actual;
}

char ***entrada_comandos(){
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

            comandos[comando][elemento] = (char*)malloc(sizeof(char) * (strlen(token_elementos) + 1));

            strcpy(comandos[comando][elemento],token_elementos);
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
    for(int i=0;comandos_anteriores[i] != NULL;i++) {
        for(int j=0;comandos_anteriores[i][j] != NULL;j++) free(comandos_anteriores[i][j]);
        free(comandos_anteriores[i]);
    }
    free(comandos_anteriores);
}

// Manejador de signals
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

// Manejar comandos internos propios de esta SHELL
int Manejar_comandos_internos(char **comando){
    if(comando[1] == NULL && strcmp(comando[0], "exit") == 0){     //Si se escribe "exit" se termina ded ejecutar el programa
        raise(SIGTERM);
    }

    if(strcmp(comando[0], "!!") == 0){
        if(comandos_anteriores == NULL) return 1;
        for(int i=0;comandos_anteriores[i] != NULL;i++) {
            for(int j=0;comandos_anteriores[i][j] != NULL;j++) printf("%s ",comandos_anteriores[i][j]);
            printf("\n");
        }
        return 1;
    }

    if(strcmp(comando[0], "cd") == 0){
        if(comando[1] == NULL) printf(ROJO "FALTA UN ARGUMENTO" RESET_COLOR "\n");
        else if (strcmp(comando[1], "~") == 0) {   //Implementacion del comando para dirigirse al Directorio Raiz
            char *home_dir = getenv("HOME");
            if (home_dir != NULL) {
                if (chdir(home_dir) != 0) printf(ROJO "Error al ingresar al Directorio HOME" RESET_COLOR "\n");
            }
            else printf(ROJO "Variable de entorno HOME no está definida" RESET_COLOR "\n");
        }
        else if (chdir(comando[1]) != 0) perror(ROJO "Error al ingresar al Directorio" RESET_COLOR);
        
        return 1;
    }
    return 0;   //No encontro ningun Comando Interno
}

void Manejar_comandos_externos(char **comando){
    pid_t child_pid = fork();

    // Proceso hijo
    if(child_pid == 0){
        if (execvp(comando[0], comando) == -1) perror(ROJO "Error en execvp");
        exit(EXIT_FAILURE);
    }
    // Proceso padre
    else if (child_pid > 0) waitpid(child_pid, NULL, 0);
    else {
        perror(ROJO "Error al crear proceso hijo" RESET_COLOR);
        liberar_comandos();
        exit(EXIT_FAILURE);
    }
}

void guardar_comandos(){
    if(strcmp(comandos[0][0],"!!") == 0) return;
    
    if(comandos_anteriores != NULL) liberar_comandos_anteriores();
    comandos_anteriores = NULL;

    int num_comandos = 0;

    while(comandos[num_comandos] != NULL) {   // Comandos

        comandos_anteriores = (char***)realloc(comandos_anteriores,sizeof(char**) * (num_comandos+1));
        
        if(comandos_anteriores == NULL){
            perror(ROJO "Error en la reasignación de memoria");
            liberar_comandos();
            exit(EXIT_FAILURE);
        }

        comandos_anteriores[num_comandos]= NULL;

        int num_elementos = 0;

        while(comandos[num_comandos][num_elementos] != NULL){    // Elementos del Comando
            comandos_anteriores[num_comandos] = (char**)realloc(comandos_anteriores[num_comandos],sizeof(char*) * (num_elementos+1));
            
            if(comandos_anteriores[num_comandos] == NULL){
                perror(ROJO "Error en la reasignación de memoria");
                exit(EXIT_FAILURE);
            }

            comandos_anteriores[num_comandos][num_elementos] = strdup(comandos[num_comandos][num_elementos]);
            num_elementos++;
        }
        comandos_anteriores[num_comandos] = (char**)realloc(comandos_anteriores[num_comandos],sizeof(char*) * (num_elementos+1));
        comandos_anteriores[num_comandos][num_elementos] = NULL;
        num_comandos++;
    }
    comandos_anteriores = (char***)realloc(comandos_anteriores,sizeof(char**) * (num_comandos+1));
    comandos_anteriores[num_comandos] = NULL;
}

int main(){
    signal(SIGINT, sig_handler);
    signal(SIGCHLD, sig_handler);
    signal(SIGTERM, sig_handler);

    while(1){
        comandos = entrada_comandos();
        if(comandos == NULL) continue;

        for(int i=0;comandos[i] != NULL;i++){
            if(Manejar_comandos_internos(comandos[i])) continue;   // Si se ejecuto un comando interno que no trate de ejecutar uno externo 

            Manejar_comandos_externos(comandos[i]);
        }

        guardar_comandos();
        liberar_comandos();
    }

    return 0;
}