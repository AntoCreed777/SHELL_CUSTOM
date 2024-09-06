#pragma once
#include <sys/types.h>

extern char ***comandos;
extern char ***cache_comandos;
extern char ***comandos_anteriores;
extern pid_t c_pid;
extern int indice_inicio_anterior_linea_comando;


extern char *archivo_favs;  //Archivo donde se guarda la lista de favoritos
extern const char *direccion_favs;  //Archivo donde se guarda la direccion del archivo de favoritos



void liberar_comandos();
void liberar_cache();
void liberar_comandos_anteriores();