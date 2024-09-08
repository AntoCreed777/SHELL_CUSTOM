#pragma once
#include <sys/types.h>
#include <stdbool.h>

extern char ***comandos;
extern char ***cache_comandos;
extern char ***comandos_anteriores;
extern bool *comandos_validos;

extern pid_t c_pid;


extern char *archivo_favs;  //Archivo donde se guarda la lista de favoritos
extern char *direccion_favs;  //Archivo donde se guarda la direccion del archivo de favoritos



void liberar_comandos();
void liberar_cache();
void liberar_comandos_anteriores();

void liberar_memoria_programa();