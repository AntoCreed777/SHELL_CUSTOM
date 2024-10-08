#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include "internos_comandos.h"
#include "constantes.h"
#include "memoria_comandos.h"
#include "utils.h"
#include "interfaz.h"
#include "externos_comandos.h"
#include "favs.h"


//Comandos internos utilizados por la shell
static int timer_command(char **comando, int num_comandos) {
    if (comando[1] == NULL) {
        printf(ROJO "FALTAN ARGMENTOS" RESET_COLOR "\n");
        comandos_validos[num_comandos] = false;
    }
    
    else if (strcmp(comando[1], "recordatorio") != 0) {
        printf(ROJO "COMANDO INVALIDO" RESET_COLOR "\n");
        comandos_validos[num_comandos] = false;
    }

    else if (comando[2] == NULL) {
        printf(ROJO "FALTAN ARGMENTOS" RESET_COLOR "\n");
        comandos_validos[num_comandos] = false;
    }

    else if (strcmp(comando[2], "help") == 0) {
        printf("set recordatorio [time] [mensaje]\n"); // Ayuda de como es la estructura del comando
        comandos_validos[num_comandos] = true;
    }

    else if (comando[3] == NULL) {
        printf(ROJO "FALTAN ARGMENTOS" RESET_COLOR "\n");
        comandos_validos[num_comandos] = false;
    }

    else if (atoi(comando[2]) <= 0) {
        printf(ROJO "DEBE SER UN NUMERO MAYOR A 0 PARA INDICAR TIEMPO" RESET_COLOR "\n");
        comandos_validos[num_comandos] = false;
    }

    else {
        pid_t pid = fork();

        if (pid == 0) { // Proceso hijo
            sleep(atoi(comando[2]));
            printf("\n" AMARILLO "ALARMA" RESET_COLOR ": " MAGENTA);
            int contador = 3;
            while (comando[contador] != NULL)
                printf("%s ", comando[contador++]);
            printf("\n");
            mostrar_prompt();
            liberar_memoria_programa();
            exit(0);
        }
        else if (pid < 0) {
            perror(ROJO "Error al crear el proceso hijo" RESET_COLOR);
            return 1;
        }

        printf(AMARILLO "Alarma configurada para dentro de %d segundos." RESET_COLOR "\n" RESET_COLOR, atoi(comando[2]));
        comandos_validos[num_comandos] = true;
    }

    return 1;
}

static int change_directory_command(char **comando, int num_comandos) {
    if (comando[1] == NULL) {
        printf(ROJO "FALTA UN ARGUMENTO" RESET_COLOR "\n");
        comandos_validos[num_comandos] = false;
    }
    else if (strcmp(comando[1], "~") == 0) { // Implementacion del comando para dirigirse al Directorio Raiz
        char *home_dir = getenv("HOME");
        if (home_dir != NULL) {
            if (chdir(home_dir) != 0){
                printf(ROJO "Error al ingresar al Directorio HOME" RESET_COLOR "\n");
                if(num_comandos != -1) comandos_validos[num_comandos] = false;
            }
        }
        else {
            printf(ROJO "Variable de entorno HOME no está definida" RESET_COLOR "\n");
            if(num_comandos != -1) comandos_validos[num_comandos] = false;
        }
    }
    else if (chdir(comando[1]) != 0){
        printf(ROJO "Error al ingresar al Directorio\n" RESET_COLOR);
        if(num_comandos != -1) comandos_validos[num_comandos] = false;
    }
    else if(num_comandos != -1) comandos_validos[num_comandos] = true;

    return 1;
}

static int usar_comando_anterior() {
    if(comandos_anteriores == NULL) return 1;

    //Impresion de los comandos anteriores
    for(int i=0;comandos_anteriores[i] != NULL;i++) {
        printf(AZUL "Command %d: " RESET_COLOR, i + 1);
        for(int j=0;comandos_anteriores[i][j] != NULL;j++)
            printf(ROJO "%s " RESET_COLOR, comandos_anteriores[i][j]);
        printf("\n");
    }

    //Ejecucion de los comandos anteriores

    for(int i=0;comandos_anteriores[i] != NULL;i++)
        manejar_comandos_externos(comandos_anteriores[i], -1);

    return 1;
}

static int favs_command(char **comando) {
    if (comando[1] == NULL) printf(ROJO "FALTAN ARGMENTOS" RESET_COLOR "\n");

    else if (strcmp(comando[1], "help") == 0)
    {
        printf(AMARILLO "Lista de argumentos disponibles:\n" AZUL " crear " ROJO " ruta/ejemplo.csv\n" AZUL " mostrar\n"
                        " eliminar " ROJO "num1,num2\n" AZUL " buscar " ROJO "cmd\n" AZUL " borrar\n" AZUL " ejecutar " ROJO "num\n" AZUL " cargar\n" AZUL " guardar\n" RESET_COLOR);
    }

    else if (strcmp(comando[1], "crear") == 0)
    { // Crea archivo donde se almacenan los comandos favoritos
        if (comando[2] == NULL){
            printf(ROJO "FALTAN ARGMENTOS" RESET_COLOR "\n");
            return 1;
        }

        FILE *file = fopen(comando[2], "w");
        if (file == NULL) {
            printf(ROJO "Error al crear el archivo" RESET_COLOR "\n");
            return 1;
        }
        
        fclose(file);
        
        if (comando[2][0] != '/') {
            char *ruta = getcwd(NULL, 0);

            if (archivo_favs != NULL) free(archivo_favs);
            archivo_favs = (char *)malloc(strlen(ruta) + strlen(comando[2]) + 2);
            if (archivo_favs == NULL) {
                perror(ROJO "Error al guardar la ruta del archivo de favoritos" RESET_COLOR);
                return 1;
            }

            strcpy(archivo_favs, ruta);
            strcat(archivo_favs, "/");
            strcat(archivo_favs, comando[2]);
            free(ruta);
        }
        else {
            archivo_favs = strdup(comando[2]);
            if (archivo_favs == NULL) {
                perror(ROJO "Error al guardar la ruta del archivo de favoritos" RESET_COLOR);
                return 1;
            }
        }

        guardar_ruta_favs();
        printf(AMARILLO "Archivo creado exitosamente" RESET_COLOR "\n");
        return 1;
    }

    else if (strcmp(comando[1], "mostrar") == 0)
    { // favs mostrar (despliega la lista comandos existentes en la lista con su respectivo número)
        mostrar_favs();
    }

    else if (strcmp(comando[1], "eliminar") == 0)
    { // favs eliminar num1,num2 (Eliminar comandos asociados a los números entregados entrecomas)
        if (comando[2] == NULL)
        {
            printf(ROJO "FALTAN ARGMENTOS" RESET_COLOR "\n");
            return 1;
        }
        // Arreglo para guardar los elementos a borrar
        int *numero_eliminar = (int *)malloc(sizeof(int));
        int contador_num_eliminar = 2;

        while (comando[contador_num_eliminar] != NULL)
        {
            if (contador_num_eliminar > 2)
                numero_eliminar = (int *)realloc(numero_eliminar, (contador_num_eliminar - 2) * sizeof(int));
            
            numero_eliminar[contador_num_eliminar - 2] = atoi(comando[contador_num_eliminar]);
            contador_num_eliminar++;
        }
        eliminar_favs(numero_eliminar, contador_num_eliminar - 2);
    }

    else if (strcmp(comando[1], "buscar") == 0)
    { // favs buscar cmd (Busca comandos que contengan substring cmd en la lista de favoritos y los despliega en pantalla junto con su número asociado)
        if (comando[2] == NULL) {
            printf(ROJO "FALTAN ARGMENTOS" RESET_COLOR "\n");
            return 1;
        }
        buscar_favs(comando[2]);
    }

    else if (strcmp(comando[1], "borrar") == 0)
    { // favs borrar (Borra todos los comandos en la lista de favoritos)
        borrar_favs();
    }

    else if (strcmp(comando[1], "ejecutar") == 0)
    { // favs num ejecutar (Ejecuta el comando, cuyo número en la lista es num) //INVERTI EL ORDEN DE LOS ARGUMENTOS
        if (comando[2] == NULL) {
            printf(ROJO "FALTAN ARGMENTOS" RESET_COLOR "\n");
            return 1;
        }
        int numero = atoi(comando[2]);
        ejecutar_favs(numero);
    }

    else if (strcmp(comando[1], "cargar") == 0)
    { // favs cargar (Lee comandos de archivo de favoritos, los mantiene en memoria y los despliega en pantalla)
        if(!cargar_favs()) return 1;
        mostrar_favs();
    }

    else if (strcmp(comando[1], "guardar") == 0)
    { // favs guardar (Guarda comandos agregados en sesión de shell actual)
        guardar_favs();
    }

    else printf(ROJO "Argumentos ingresados erroneos" RESET_COLOR "\n");

    return 1;
}

// Manejar comandos internos propios de esta SHELL
int manejar_comandos_internos(char **comando, int num_comandos) {
    if(strcmp(comando[0], "exit") == 0) {     //Si se escribe "exit" se termina ded ejecutar el programa
        raise(SIGTERM);
    }

    if(strcmp(comando[0], "!!") == 0) {
        return usar_comando_anterior();
    }

    if(strcmp(comando[0], "cd") == 0) {
        return change_directory_command(comando, num_comandos);
    }

    if(strcmp(comando[0], "set") == 0) {
        return timer_command(comando, num_comandos);
    }

    if(strcmp(comando[0], "favs") == 0) {
        return favs_command(comando);
    }

    return 0;   //No encontro ningun Comando Interno
}

