#pragma once


extern char ***comandos;
extern char ***comandos_anteriores;

char *directorio_actual();

char *usuario_actual();

char ***entrada_comandos();

void liberar_comandos();

void liberar_comandos_anteriores();
