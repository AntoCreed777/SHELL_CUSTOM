#pragma once

extern char ***comandos;
extern char ***comandos_anteriores;


char ***entrada_comandos();

void liberar_comandos();

void liberar_comandos_anteriores();

void pipeling(char **comando, int posicion_pipeling);
void manejar_comandos_externos(char **comando);
int manejar_comandos_internos(char **comando);
void guardar_comandos();
