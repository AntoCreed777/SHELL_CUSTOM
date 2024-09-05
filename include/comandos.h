#pragma once

extern char ***comandos;

char ***entrada_comandos();

void liberar_comandos();

void liberar_cache();

void manejar_comandos_externos(char **comando);
int manejar_comandos_internos(char **comando);
void guardar_comandos();

